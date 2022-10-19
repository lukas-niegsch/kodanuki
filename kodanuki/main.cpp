#include "pipelines/example.h"
#include "plugin/vulkan/debug.h"
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/pipeline.h"
#include "plugin/vulkan/renderer.h"
#include "plugin/vulkan/shader.h"
#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/window.h"
using namespace Kodanuki;

constexpr float cube_strip[] = {
	 1,  1,  1,
	-1,  1,  1,
	 1, -1,  1,
	-1, -1,  1,
	-1, -1, -1,
	-1,  1,  1,
	-1,  1, -1,
	 1,  1,  1,
	 1,  1, -1,
	 1, -1,  1,
	 1, -1, -1,
	-1, -1, -1,
	 1,  1, -1,
	-1,  1, -1
};

WindowBuilder get_window_builder(std::string title, int width, int height)
{
	return {
		.title = title,
		.default_size = {(uint32_t) width, (uint32_t) height},
		.resizeable = false
	};
}

int score_physical_device(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

int score_queue_family(VkQueueFamilyProperties family)
{
	int score = family.queueCount;
	score *= family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	return score;
}

DeviceBuilder get_device_builder(VulkanWindow window)
{
	return {
		.instance_layers = {"VK_LAYER_KHRONOS_validation"},
		.instance_extensions = window.required_instance_extensions(),
		.device_extensions = {"VK_KHR_swapchain"},
		.gpu_score = &score_physical_device,
		.queue_score = &score_queue_family,
		.queue_priorities = {1.0f}
	};
}

SwapchainBuilder get_swapchain_builder(VulkanDevice device, VulkanWindow window)
{
	return {
		.device = device,
		.surface = window.create_surface(device),
		.surface_format = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		.present_mode = VK_PRESENT_MODE_FIFO_KHR,
		.frame_count = 2
	};
}

RendererBuilder get_renderer_builder(VulkanDevice device, VulkanSwapchain swapchain, VulkanRenderpass renderpass)
{
	return {
		.device = device,
		.swapchain = swapchain,
		.renderpass = renderpass,
		.command_buffer_count = 1,
		.clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}}
	};
}

int main()
{
	VulkanWindow window = {get_window_builder("Kodanuki", 1024, 768)};
	VulkanDevice device = {get_device_builder(window)};
	VulkanSwapchain swapchain = {get_swapchain_builder(device, window)};
	VulkanRenderpass renderpass = get_example_triangle_renderpass(device, swapchain);
	VulkanPipeline pipeline = get_example_triangle_pipeline(device, renderpass);
	VulkanRenderer renderer = {get_renderer_builder(device, swapchain, renderpass)};
	
	auto record_pipeline = [&](VkCommandBuffer buffer) {
		
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline());

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = swapchain.surface_extent().width;
		viewport.height = swapchain.surface_extent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(buffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = swapchain.surface_extent();
		vkCmdSetScissor(buffer, 0, 1, &scissor);

		vkCmdDraw(buffer, 3, 1, 0, 0);
	};

	while (!window.should_close())
	{
		renderer.aquire_next_frame();
		renderer.record_command_buffer(record_pipeline);
		renderer.submit_command_buffers();
		renderer.render_next_frame();
	}

	return 0;
}
