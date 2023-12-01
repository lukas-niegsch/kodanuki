#include "engine/display/vkdraw.h"
#include <ranges>


namespace kodanuki::vkdraw
{

/**
 * The timeout for drawing commands in (almost) nanoseconds.
 */
static uint64_t TIMEOUT = 1 /* seconds */ * 100000000;

void aquire_frame(VulkanDevice device, VulkanWindow& window)
{
	uint32_t current_frame = window.submit_frame;
	
	CHECK_VULKAN(vkWaitForFences(device, 1,
		window.aquire_frame_fences[current_frame], VK_TRUE, TIMEOUT));

	auto result = vkAcquireNextImageKHR(device, window.swapchain, TIMEOUT,
		window.image_available_semaphores[current_frame], VK_NULL_HANDLE,
		&window.render_frame);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		window.recreate(device);
		aquire_frame(device, window);
		return;
	}

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to acquire swap chain image.");
	}

	CHECK_VULKAN(vkResetFences(device, 1,
		window.aquire_frame_fences[current_frame]));
}

void record_frame(VulkanDevice device, VulkanWindow& window, std::vector<fn_draw> commands)
{
	uint32_t current_frame = window.submit_frame;
	VkCommandBuffer buffer = window.render_buffers[current_frame];

	VkCommandBufferBeginInfo buffer_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pInheritanceInfo = nullptr
	};
	CHECK_VULKAN(vkResetCommandBuffer(buffer, 0));
	CHECK_VULKAN(vkBeginCommandBuffer(buffer, &buffer_info));

	VkImageMemoryBarrier initial_image_memory_barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.srcQueueFamilyIndex = device.hardware.queue_family_index,
		.dstQueueFamilyIndex = device.hardware.queue_family_index,
		.image = window.render_images[current_frame],
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
	};
	vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr,
		0, nullptr, 1, &initial_image_memory_barrier);

	VkRenderingAttachmentInfo color_attachment = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.pNext = nullptr,
		.imageView = window.render_image_views[current_frame],
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
		.resolveMode = VK_RESOLVE_MODE_NONE,
		.resolveImageView = {},
		.resolveImageLayout = {},
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = {0.53f, 0.81f, 0.92f},
	};
	VkRenderingAttachmentInfo depth_attachment = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.pNext = nullptr,
		.imageView = window.depth_image_view,
		.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.resolveMode = VK_RESOLVE_MODE_NONE,
		.resolveImageView = {},
		.resolveImageLayout = {},
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.clearValue = {1.0f, 0},
	};
	VkRenderingInfo render_info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.renderArea = {{0, 0}, window.surface_extent},
		.layerCount = 1,
		.viewMask = 0,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment,
		.pDepthAttachment = &depth_attachment,
		.pStencilAttachment = nullptr,
	};
	vkCmdBeginRendering(buffer, &render_info);

	// TODO: This is bad design, we give the user a method
    //       that must be called after vkCmdBindPipeline ...
	auto set_dynamic_state = [&]() {
		VkViewport keep_everything_viewport = {
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(window.surface_extent.width),
			.height = static_cast<float>(window.surface_extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
		vkCmdSetViewport(buffer, 0, 1, &keep_everything_viewport);

		VkRect2D keep_everything_scissor = {
			.offset = {0, 0},
			.extent = window.surface_extent,
		};
		vkCmdSetScissor(buffer, 0, 1, &keep_everything_scissor);
	};

	for (auto command : commands) {
		command(buffer, set_dynamic_state);
	}

	vkCmdEndRendering(buffer);

	VkImageMemoryBarrier final_image_memory_barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		.dstAccessMask = 0,
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = device.hardware.queue_family_index,
		.dstQueueFamilyIndex = device.hardware.queue_family_index,
		.image = window.render_images[current_frame],
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
	};
	vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1,
		&final_image_memory_barrier);

	CHECK_VULKAN(vkEndCommandBuffer(buffer));
}

void submit_frame(VulkanDevice device, VulkanWindow& window, uint32_t queue_index)
{
	uint32_t current_frame = window.submit_frame;
	VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = window.image_available_semaphores[current_frame],
		.pWaitDstStageMask = &flags,
		.commandBufferCount = 1,
		.pCommandBuffers = window.render_buffers[current_frame],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = window.render_finished_semaphores[current_frame],
	};

	VkQueue submit_queue;
	vkGetDeviceQueue(device, device.hardware.queue_family_index, queue_index, &submit_queue);

	CHECK_VULKAN(vkQueueSubmit(submit_queue, 1, &info, window.aquire_frame_fences[current_frame]));
}

void render_frame(VulkanDevice device, VulkanWindow& window, uint32_t queue_index)
{
	uint32_t current_frame = window.submit_frame;

	VkPresentInfoKHR info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = window.render_finished_semaphores[current_frame],
		.swapchainCount = 1,
		.pSwapchains = window.swapchain,
		.pImageIndices = &window.render_frame,
		.pResults = nullptr,
	};

	VkQueue present_queue;
	vkGetDeviceQueue(device, device.hardware.queue_family_index, queue_index, &present_queue);

	auto result = vkQueuePresentKHR(present_queue, &info);
	window.submit_frame = (current_frame + 1) % window.image_specs.frame_count;

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		window.recreate(device);
		return;
	}

	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}
}

fn_draw indexed(const DrawIndexedParams& params)
{
	return [=](VkCommandBuffer buffer, std::function<void()> set_pipeline_state) {
		std::vector<VkBuffer> vertex_buffers;
		std::vector<VkDeviceSize> vertex_offsets;
		for (auto tensor : params.vertices) {
			vertex_buffers.push_back(tensor.staging_buffer);
			vertex_offsets.push_back(0);
		}
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, params.pipeline);
		set_pipeline_state();
		vkCmdBindVertexBuffers(buffer, 0, vertex_buffers.size(), vertex_buffers.data(), vertex_offsets.data());
		vkCmdBindIndexBuffer(buffer, params.indices.staging_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(buffer, params.index_count, params.instance_count, 0, 0, 0);
	};
}

}
