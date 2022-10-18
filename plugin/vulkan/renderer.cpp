#include "plugin/vulkan/renderer.h"
#include "plugin/vulkan/debug.h"

namespace Kodanuki
{

std::vector<VkFramebuffer> create_frame_buffers(RendererBuilder builder)
{
	std::vector<VkImageView> views = builder.swapchain.image_views();
	std::vector<VkFramebuffer> framebuffers(views.size());
	VkDevice logical_device = builder.device.logical_device();
	VkExtent2D size = builder.swapchain.surface_extent();

	VkFramebufferCreateInfo framebuffer_info = {};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.renderPass = builder.renderpass.renderpass();
	framebuffer_info.layers = 1;
	framebuffer_info.width = size.width;
	framebuffer_info.height = size.height;

	for (uint32_t i = 0; i < framebuffers.size(); i++) {
		std::vector<VkImageView> attachments = {views[i]};
		framebuffer_info.attachmentCount = attachments.size();
		framebuffer_info.pAttachments = attachments.data();

		CHECK_VULKAN(vkCreateFramebuffer(logical_device, &framebuffer_info, nullptr, &framebuffers[i]));
	}

	return framebuffers;
}

std::vector<VkCommandBuffer> create_command_buffers(RendererBuilder builder)
{
	VkCommandBufferAllocateInfo buffer_info;
    buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_info.pNext = nullptr;
    buffer_info.commandPool = builder.device.command_pool();
    buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_info.commandBufferCount = builder.command_buffer_count;

	VkDevice device = builder.device.logical_device();
	std::vector<VkCommandBuffer> command_buffers(builder.command_buffer_count);
	CHECK_VULKAN(vkAllocateCommandBuffers(device, &buffer_info, command_buffers.data()));
	return command_buffers;
}

class RenderValues
{
public:
	RenderValues(RendererBuilder builder)
		: device(builder.device)
		, swapchain(builder.swapchain)
		, renderpass(builder.renderpass)
	{
		current_frame = 0;
		frame_count = builder.swapchain.frame_count();
		frame_buffers = create_frame_buffers(builder);
		command_buffers = create_command_buffers(builder);
		clear_color = builder.clear_color;
		stage_masks = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		create_synchronization_objects();
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
			CHECK_VULKAN(vkCreateSemaphore(device.logical_device(), &semaphore_info, nullptr, &image_available_semaphores[i]));
			CHECK_VULKAN(vkCreateSemaphore(device.logical_device(), &semaphore_info, nullptr, &render_finished_semaphores[i]));
			CHECK_VULKAN(vkCreateFence(device.logical_device(), &fence_info, nullptr, &draw_frame_fences[i]));
		}
	}

	void destroy()
	{
		for (VkSemaphore semaphore : image_available_semaphores) {
			vkDestroySemaphore(device.logical_device(), semaphore, nullptr);
		}
		for (VkSemaphore semaphore : render_finished_semaphores) {
			vkDestroySemaphore(device.logical_device(), semaphore, nullptr);
		}
		for (VkFence fence : draw_frame_fences) {
			vkDestroyFence(device.logical_device(), fence, nullptr);
		}
		for (VkFramebuffer frame_buffer : frame_buffers) {
			vkDestroyFramebuffer(device.logical_device(), frame_buffer, nullptr);
		}
	}

public:
	VulkanDevice device;
	VulkanSwapchain swapchain;
	VulkanRenderpass renderpass;
	uint32_t current_frame;
	uint32_t frame_count;
	std::vector<VkFramebuffer> frame_buffers;
	std::vector<VkCommandBuffer> command_buffers;
	VkClearValue clear_color;
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
	uint32_t frame = values.current_frame;
	VkDevice device = values.device.logical_device();

	VkFence next_fence = values.draw_frame_fences[frame]; 
	CHECK_VULKAN(vkWaitForFences(device, 1, &next_fence, VK_TRUE, UINT64_MAX));
	CHECK_VULKAN(vkResetFences(device, 1, &next_fence));

	for (VkCommandBuffer buffer : values.command_buffers) {
		vkResetCommandBuffer(buffer, 0);
	}

	VkSwapchainKHR swapchain = values.swapchain.swapchain();
	VkSemaphore next_semaphore = values.image_available_semaphores[frame];
	vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, next_semaphore, VK_NULL_HANDLE, &frame);
}

void VulkanRenderer::record_command_buffer(std::function<void(VkCommandBuffer)> callback, uint32_t buffer_index)
{
	RenderValues& values = ECS::get<RenderValues>(*pimpl);
	VkCommandBuffer current_buffer = values.command_buffers[buffer_index];
	
	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CHECK_VULKAN(vkBeginCommandBuffer(current_buffer, &command_buffer_begin_info));

	VkRenderPassBeginInfo renderpass_begin_info = {};
	renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_begin_info.renderPass = values.renderpass.renderpass();
	renderpass_begin_info.framebuffer = values.frame_buffers[values.current_frame];
	renderpass_begin_info.renderArea.offset = {0, 0};
	renderpass_begin_info.renderArea.extent = values.swapchain.surface_extent();
	renderpass_begin_info.clearValueCount = 1;
	renderpass_begin_info.pClearValues = &values.clear_color;
	
	vkCmdBeginRenderPass(current_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	callback(current_buffer);
	vkCmdEndRenderPass(current_buffer);
	CHECK_VULKAN(vkEndCommandBuffer(current_buffer));
}

void VulkanRenderer::submit_command_buffers(uint32_t queue_index)
{
	RenderValues& values = ECS::get<RenderValues>(*pimpl);
	uint32_t frame = values.current_frame;
	VkQueue queue = values.device.queues()[queue_index];

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &(values.image_available_semaphores[frame]);
	submit_info.pWaitDstStageMask = values.stage_masks.data();
	submit_info.commandBufferCount = values.command_buffers.size();
	submit_info.pCommandBuffers = values.command_buffers.data();
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &(values.render_finished_semaphores[frame]);

	CHECK_VULKAN(vkQueueSubmit(queue, 1, &submit_info, values.draw_frame_fences[frame]));
	values.current_frame = (frame + 1) % values.frame_count;
}

void VulkanRenderer::render_next_frame()
{

}

}
