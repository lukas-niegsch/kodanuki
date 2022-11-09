#include "plugin/vulkan/renderer.h"
#include "plugin/vulkan/debug.h"
#include <unordered_set>

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
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = queue_index;

	VkCommandPool command_pool;
	CHECK_VULKAN(vkCreateCommandPool(device, &pool_info, nullptr, &command_pool));
	return command_pool;
}

class SerialVulkanRenderer
{
private:
	inline static uint64_t AQUIRE_TIMEOUT = 10 /* seconds*/ * 1000000000ull;

public:
	SerialVulkanRenderer(RendererBuilder builder);
	void create_synchronization_objects();
	void destroy();

public:
	void aquire();
	void submit(uint32_t queue_index);
	void draw(VulkanPipeline pipeline, VulkanBuffer instance);
	void record();
	void render(uint32_t queue_index);

private:
	VulkanDevice device;
	VulkanSwapchain swapchain;
	VulkanRenderpass renderpass;
	uint32_t submit_frame;
	uint32_t render_frame;
	uint32_t max_frame;
	VkClearValue clear_color;
	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> command_buffers;
	std::vector<VkPipelineStageFlags> stage_masks;
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> aquire_frame_fences;
	std::vector<VulkanPipeline> models;
	std::vector<VulkanBuffer> instances;
};

SerialVulkanRenderer::SerialVulkanRenderer(RendererBuilder builder)
	: device(builder.device)
	, swapchain(builder.swapchain)
	, renderpass(builder.renderpass)
{
	submit_frame = 0;
	max_frame = swapchain.frame_count();
	clear_color = builder.clear_color;
	stage_masks = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	create_synchronization_objects();
	command_pool = create_command_pool(device, device.queue_family_index());
	command_buffers = create_command_buffers(device, command_pool, max_frame);
}

void SerialVulkanRenderer::create_synchronization_objects()
{
	image_available_semaphores.resize(max_frame);
	render_finished_semaphores.resize(max_frame);
	aquire_frame_fences.resize(max_frame);

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < max_frame; i++) {
		CHECK_VULKAN(vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_semaphores[i]));
		CHECK_VULKAN(vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_semaphores[i]));
		CHECK_VULKAN(vkCreateFence(device, &fence_info, nullptr, &aquire_frame_fences[i]));
	}
}

void SerialVulkanRenderer::destroy()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	for (VkSemaphore semaphore : image_available_semaphores) {
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	for (VkSemaphore semaphore : render_finished_semaphores) {
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	for (VkFence fence : aquire_frame_fences) {
		vkDestroyFence(device, fence, nullptr);
	}
	vkDestroyCommandPool(device, command_pool, nullptr);
}

void SerialVulkanRenderer::aquire()
{
	CHECK_VULKAN(vkWaitForFences(device, 1, &aquire_frame_fences[submit_frame], VK_TRUE, AQUIRE_TIMEOUT));

	auto result = vkAcquireNextImageKHR(device, swapchain, AQUIRE_TIMEOUT, image_available_semaphores[submit_frame],
										VK_NULL_HANDLE, &render_frame);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		swapchain.recreate_swapchain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	CHECK_VULKAN(vkResetFences(device, 1, &aquire_frame_fences[submit_frame]));
	CHECK_VULKAN(vkResetCommandBuffer(command_buffers[submit_frame], 0));
}

void SerialVulkanRenderer::submit(uint32_t queue_index)
{
	VkSubmitInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &image_available_semaphores[submit_frame];
	info.pWaitDstStageMask = stage_masks.data();
	info.commandBufferCount = 1;
	info.pCommandBuffers = &command_buffers[submit_frame];
	info.signalSemaphoreCount = 1;
	info.pSignalSemaphores = &render_finished_semaphores[submit_frame];

	VkQueue queue = device.queues()[queue_index];
	CHECK_VULKAN(vkQueueSubmit(queue, 1, &info, aquire_frame_fences[submit_frame]));
	submit_frame = (submit_frame + 1) % max_frame;
}

void SerialVulkanRenderer::draw(VulkanPipeline pipeline, VulkanBuffer instance)
{
	models.push_back(pipeline);
	instances.push_back(instance);
}

void SerialVulkanRenderer::record()
{
	// We currently only write to one command buffer per frame!
	VkCommandBuffer buffer = command_buffers[submit_frame];

	VkCommandBufferBeginInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CHECK_VULKAN(vkBeginCommandBuffer(buffer, &buffer_info));

	VkRenderPassBeginInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_info.renderPass = renderpass;
	renderpass_info.framebuffer = swapchain.frame_buffers()[submit_frame];
	renderpass_info.renderArea.offset = {0, 0};
	renderpass_info.renderArea.extent = swapchain.surface_extent();
	renderpass_info.clearValueCount = 1;
	renderpass_info.pClearValues = &clear_color;

	vkCmdBeginRenderPass(buffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

	for (int i = 0; i < (int) models.size(); i++)
	{
		VulkanPipeline model = models[i];
		VulkanBuffer instance = instances[i];

		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, model);
		VkBuffer vertexBuffers[] = {instance};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
		vkCmdDraw(buffer, instance.byte_size(), 1, 0, 0);
	}

	vkCmdEndRenderPass(buffer);
	CHECK_VULKAN(vkEndCommandBuffer(buffer));
}

void SerialVulkanRenderer::render(uint32_t queue_index)
{
	std::vector<VkSwapchainKHR> swapchains = {swapchain};

	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_finished_semaphores[render_frame];
	info.swapchainCount = swapchains.size();
	info.pSwapchains = swapchains.data();
	info.pImageIndices = &render_frame;

	VkQueue queue = device.queues()[queue_index];
	auto result = vkQueuePresentKHR(queue, &info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		swapchain.recreate_swapchain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}
}

VulkanRenderer::VulkanRenderer(RendererBuilder builder)
{
	ECS::update<SerialVulkanRenderer>(impl, {builder});
}

void VulkanRenderer::shared_destructor()
{
	ECS::get<SerialVulkanRenderer>(impl).destroy();
}

void VulkanRenderer::aquire()
{
	ECS::get<SerialVulkanRenderer>(impl).aquire();
}

void VulkanRenderer::submit(uint32_t queue_index)
{
	ECS::get<SerialVulkanRenderer>(impl).submit(queue_index);
}

void VulkanRenderer::draw(VulkanPipeline pipeline, VulkanBuffer instance)
{
	ECS::get<SerialVulkanRenderer>(impl).draw(pipeline, instance);
}

void VulkanRenderer::record()
{
	ECS::get<SerialVulkanRenderer>(impl).record();
}

void VulkanRenderer::render(uint32_t queue_index)
{
	ECS::get<SerialVulkanRenderer>(impl).render(queue_index);
}

}
