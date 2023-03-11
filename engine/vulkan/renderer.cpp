#include "engine/vulkan/renderer.h"
#include "engine/vulkan/debug.h"
#include "engine/vulkan/wrapper.h"

namespace kodanuki
{

struct RendererState
{
	VulkanContext device;
	VulkanTarget target;
	uint32_t submit_frame;
	uint32_t render_frame;
	uint32_t max_frame;
	VkClearColorValue clear_color;
	std::vector<VkPipelineStageFlags> stage_masks;
	std::vector<VulkanCommandBuffer> command_buffers;
	std::vector<VulkanSemaphore> image_available_semaphores;
	std::vector<VulkanSemaphore> render_finished_semaphores;
	std::vector<VulkanFence> aquire_frame_fences;
	VulkanCommandBuffer compute_buffer;
};

VulkanRenderer::VulkanRenderer(RendererBuilder builder)
{
	state = std::make_shared<RendererState>(builder.device, builder.target);
	state->clear_color = builder.clear_color;
	state->max_frame = builder.target.get_frame_count();
	state->stage_masks = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	state->submit_frame = 0;
	state->render_frame = 0;

	for (uint32_t i = 0; i < state->max_frame; i++) {
		state->command_buffers.push_back(create_command_buffer(state->device, state->device.get_command_pool()));
		state->image_available_semaphores.push_back(create_semaphore(state->device));
		state->render_finished_semaphores.push_back(create_semaphore(state->device));
		state->aquire_frame_fences.push_back(create_fence(state->device, VK_FENCE_CREATE_SIGNALED_BIT));
	}
	state->compute_buffer = create_command_buffer(state->device, state->device.get_command_pool());
}

uint32_t VulkanRenderer::aquire_frame()
{
	const VkFence& aquire_frame = state->aquire_frame_fences[state->submit_frame];
	CHECK_VULKAN(vkWaitForFences(state->device, 1, &aquire_frame, VK_TRUE, AQUIRE_TIMEOUT));

	const VkSemaphore& image_available = state->image_available_semaphores[state->submit_frame];
	auto result = vkAcquireNextImageKHR(state->device, state->target.swapchain(),
		AQUIRE_TIMEOUT, image_available, VK_NULL_HANDLE, &state->render_frame);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		throw std::runtime_error("swapchain recreation not implement inside renderer");
		state->target.recreate_swapchain();
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	CHECK_VULKAN(vkResetFences(state->device, 1, &aquire_frame));

	const VkCommandBuffer& command_buffer = state->command_buffers[state->submit_frame];
	CHECK_VULKAN(vkResetCommandBuffer(command_buffer, 0));
	return state->submit_frame;
}

void VulkanRenderer::draw_command(std::function<void(VkCommandBuffer)> command)
{
	const VkCommandBuffer& buffer = state->command_buffers[state->submit_frame];

	VkCommandBufferBeginInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CHECK_VULKAN(vkBeginCommandBuffer(buffer, &buffer_info));

	std::array<VkClearValue, 2> clear_values = {};
	clear_values[0].color = state->clear_color;
	clear_values[1].depthStencil = {1.0f, 0};

	VkRenderPassBeginInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_info.renderPass = state->target.renderpass();
	renderpass_info.framebuffer = state->target.get_frame_buffer(state->submit_frame);
	renderpass_info.renderArea.offset = {0, 0};
	renderpass_info.renderArea.extent = state->target.get_surface_extent();
	renderpass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
	renderpass_info.pClearValues = clear_values.data();

	vkCmdBeginRenderPass(buffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

	command(buffer);

	vkCmdEndRenderPass(buffer);
	CHECK_VULKAN(vkEndCommandBuffer(buffer));
}

void VulkanRenderer::submit_frame(uint32_t queue_index)
{
	const VkSemaphore& image_available = state->image_available_semaphores[state->submit_frame];
	const VkSemaphore& render_finished = state->render_finished_semaphores[state->submit_frame];
	const VkCommandBuffer& buffer = state->command_buffers[state->submit_frame];
	const VkFence& aquire_frame = state->aquire_frame_fences[state->submit_frame];

	VkSubmitInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &image_available;
	info.pWaitDstStageMask = state->stage_masks.data();
	info.commandBufferCount = 1;
	info.pCommandBuffers = &buffer;
	info.signalSemaphoreCount = 1;
	info.pSignalSemaphores = &render_finished;

	VkQueue queue = state->device.get_queues()[queue_index];
	CHECK_VULKAN(vkQueueSubmit(queue, 1, &info, aquire_frame));
	state->submit_frame = (state->submit_frame + 1) % state->max_frame;
}

void VulkanRenderer::render_frame(uint32_t queue_index)
{
	VkSwapchainKHR swapchain = state->target.swapchain();
	const VkSemaphore& render_finished = state->render_finished_semaphores[state->render_frame];
	
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_finished;
	info.swapchainCount = 1;
	info.pSwapchains = &swapchain;
	info.pImageIndices = &state->render_frame;

	VkQueue queue = state->device.get_queues()[queue_index];
	auto result = vkQueuePresentKHR(queue, &info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("swapchain recreation not implement inside renderer");
		state->target.recreate_swapchain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}
}

}
