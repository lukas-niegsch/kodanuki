#include "pipelines/example.h"
#include "plugin/vulkan/debug.h"
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/pipeline.h"
#include "plugin/vulkan/renderer.h"
#include "plugin/vulkan/shader.h"
#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/window.h"
#include <GLFW/glfw3.h>
#include <iomanip>
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
	VulkanWindow window = {{
		.title = "Kodanuki",
		.default_size = {1024, 768},
		.resizeable = false // TODO: renderer needs to recreate swapchain properly
	}};

	VulkanDevice device = {{
		.instance_layers = {"VK_LAYER_KHRONOS_validation"},
		.instance_extensions = window.required_instance_extensions(),
		.device_extensions = {"VK_KHR_swapchain"},
		.gpu_score = &score_physical_device,
		.queue_score = &score_queue_family,
		.queue_priorities = {1.0f}
	}};

	// TODO: make generic renderpass and include it inside the plugin
	VulkanRenderpass renderpass = get_example_triangle_renderpass(device);

	VulkanSwapchain swapchain = {{
		.device = device,
		.renderpass = renderpass,
		.surface = window.create_surface(device),
		.surface_format = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		.present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR,
		.frame_count = 3
	}};

	VulkanPipeline pipeline = get_example_triangle_pipeline(device, swapchain, renderpass);
	VulkanRenderer renderer = {get_renderer_builder(device, swapchain, renderpass)};

	auto record_pipeline = [&](VkCommandBuffer buffer) {
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline());
		vkCmdDraw(buffer, 3, 1, 0, 0);
	};

	std::cout << std::endl;
	print_vulkan_info(device.physical_device());
	
	std::cout << "Average time per frame: " << '\n';
	std::cout << std::setprecision(4);
	double previous_time = glfwGetTime();
	uint32_t frame_count = 0;

	while (!window.should_close())
	{
		double current_time = glfwGetTime();
		frame_count++;

		if (current_time - previous_time >= 1.0) {
			std::cout << 1000.0 / frame_count << " ms" << '\n';
			frame_count = 0;
			previous_time = current_time;
		}

		renderer.aquire_next_frame();
		renderer.record_command_buffer(record_pipeline);
		renderer.submit_command_buffers();
		renderer.render_next_frame();
	}

	return 0;
}
