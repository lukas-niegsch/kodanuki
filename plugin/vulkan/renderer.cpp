#include "plugin/vulkan/renderer.h"
#include "plugin/vulkan/debug.h"

namespace Kodanuki
{

std::vector<VkCommandBuffer> create_command_buffers(VkDevice device, VkCommandPool pool, uint32_t count)
{
	VkCommandBufferAllocateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	buffer_info.commandPool = pool;
	buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	buffer_info.commandBufferCount = count;

	std::vector<VkCommandBuffer> result(count);
	CHECK_VULKAN(vkAllocateCommandBuffers(device, &buffer_info, result.data()));
	return result;
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_index)
{
	VkCommandPoolCreateInfo pool_info;
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.pNext = nullptr;
	pool_info.flags = 0;
	pool_info.queueFamilyIndex = queue_index;

	VkCommandPool command_pool;
	CHECK_VULKAN(vkCreateCommandPool(device, &pool_info, nullptr, &command_pool));
	return command_pool;
}

class RenderValues
{
public:
	RenderValues(RendererBuilder builder)
		: device(builder.device)
		, swapchain(builder.swapchain)
		, renderpass(builder.renderpass)
	{
		submit_frame = 0;
		frame_count = builder.swapchain.frame_count();
		clear_color = builder.clear_color;
		stage_masks = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		create_synchronization_objects();
		command_pool = create_command_pool(device, device.queue_family_index());
	}

	void create_synchronization_objects()
	{
		image_available_semaphores.resize(frame_count);
		render_finished_semaphores.resize(frame_count);
		draw_frame_fences.resize(frame_count);

		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < frame_count; i++) {
			CHECK_VULKAN(vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_semaphores[i]));
			CHECK_VULKAN(vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_semaphores[i]));
			CHECK_VULKAN(vkCreateFence(device, &fence_info, nullptr, &draw_frame_fences[i]));
		}
	}

	void destroy()
	{
		CHECK_VULKAN(vkDeviceWaitIdle(device));
		for (VkSemaphore semaphore : image_available_semaphores) {
			vkDestroySemaphore(device, semaphore, nullptr);
		}
		for (VkSemaphore semaphore : render_finished_semaphores) {
			vkDestroySemaphore(device, semaphore, nullptr);
		}
		for (VkFence fence : draw_frame_fences) {
			vkDestroyFence(device, fence, nullptr);
		}
		vkDestroyCommandPool(device, command_pool, nullptr);
	}

public:
	VulkanDevice device;
	VulkanSwapchain swapchain;
	VulkanRenderpass renderpass;
	uint32_t submit_frame;
	uint32_t render_image;
	uint32_t frame_count;
	std::vector<VkCommandBuffer> command_buffers;
	VkClearValue clear_color;
	VkCommandPool command_pool;
	std::vector<VkPipelineStageFlags> stage_masks;
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> draw_frame_fences;
};

void remove_renderer(Entity* renderer)
{
	ECS::get<RenderValues>(*renderer).destroy();
	ECS::remove<Entity>(*renderer);
	delete renderer;
}

VulkanRenderer::VulkanRenderer(RendererBuilder builder)
{
	pimpl = std::shared_ptr<Entity>(new Entity, &remove_renderer);
	Entity renderer = *pimpl = ECS::create();
	ECS::update<RenderValues>(renderer, {builder});
}

void VulkanRenderer::aquire_next_frame()
{
	RenderValues& values = ECS::get<RenderValues>(*pimpl);
	VkDevice device = values.device;
	VkFence draw_fence = values.draw_frame_fences[values.submit_frame]; 
	CHECK_VULKAN(vkWaitForFences(device, 1, &draw_fence, VK_TRUE, UINT64_MAX));
	CHECK_VULKAN(vkResetFences(device, 1, &draw_fence));
	VkSwapchainKHR swapchain = values.swapchain;
	VkSemaphore image_semaphore = values.image_available_semaphores[values.submit_frame];
	vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_semaphore, VK_NULL_HANDLE, &values.render_image);
	values.command_buffers.clear();
}

void VulkanRenderer::record_command_buffer(std::function<void(VkCommandBuffer)> callback)
{
	RenderValues& values = ECS::get<RenderValues>(*pimpl);
	VkDevice device = values.device;
	VkCommandPool pool = values.command_pool;
	VkCommandBuffer command_buffer = create_command_buffers(device, pool, 1)[0];

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CHECK_VULKAN(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

	VkRenderPassBeginInfo renderpass_begin_info = {};
	renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_begin_info.renderPass = values.renderpass;
	renderpass_begin_info.framebuffer = values.swapchain.frame_buffers()[values.submit_frame];
	renderpass_begin_info.renderArea.offset = {0, 0};
	renderpass_begin_info.renderArea.extent = values.swapchain.surface_extent();
	renderpass_begin_info.clearValueCount = 1;
	renderpass_begin_info.pClearValues = &values.clear_color;
	
	vkCmdBeginRenderPass(command_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	callback(command_buffer);
	vkCmdEndRenderPass(command_buffer);

	CHECK_VULKAN(vkEndCommandBuffer(command_buffer));
	values.command_buffers.push_back(command_buffer);
}

void VulkanRenderer::submit_command_buffers(uint32_t queue_index)
{
	RenderValues& values = ECS::get<RenderValues>(*pimpl);
	VkQueue queue = values.device.queues()[queue_index];

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &values.image_available_semaphores[values.submit_frame];
	submit_info.pWaitDstStageMask = values.stage_masks.data();
	submit_info.commandBufferCount = values.command_buffers.size();
	submit_info.pCommandBuffers = values.command_buffers.data();
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &values.render_finished_semaphores[values.submit_frame];

	CHECK_VULKAN(vkQueueSubmit(queue, 1, &submit_info, values.draw_frame_fences[values.submit_frame]));
	values.submit_frame = (values.submit_frame + 1) % values.frame_count;
}

void VulkanRenderer::render_next_frame(uint32_t queue_index)
{
	RenderValues& values = ECS::get<RenderValues>(*pimpl);
	VkQueue queue = values.device.queues()[queue_index];
	VkSwapchainKHR swapchain = values.swapchain;

	VkPresentInfoKHR present_Info = {};
	present_Info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_Info.waitSemaphoreCount = 1;
	present_Info.pWaitSemaphores = &values.render_finished_semaphores[values.render_image];
	present_Info.swapchainCount = 1;
	present_Info.pSwapchains = &swapchain;
	present_Info.pImageIndices = &values.render_image;
	present_Info.pResults = nullptr;

	vkQueuePresentKHR(queue, &present_Info);
}

}
