#include "pipelines/example.h"
#include "plugin/vulkan/debug.h"
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/pipeline.h"
#include "plugin/vulkan/shader.h"
#include "plugin/vulkan/swapchain.h"
#include "engine/utility/file.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
using namespace Kodanuki;

constexpr GLfloat cube_strip[] = {
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

GLFWwindow* create_default_window(int width, int height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(width, height, "", NULL, NULL);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	return window;
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

DeviceBuilder get_device_builder()
{
	uint32_t count;
	const char** extensions = glfwGetRequiredInstanceExtensions(&count);

	return {
		.instance_layers = {"VK_LAYER_KHRONOS_validation"},
		.instance_extensions = std::vector(extensions, extensions + count),
		.device_extensions = {"VK_KHR_swapchain"},
		.gpu_score = &score_physical_device,
		.queue_score = &score_queue_family,
		.queue_priorities = {1.0f}
	};
}

SwapchainBuilder get_swapchain_builder(VulkanDevice device, GLFWwindow* window)
{
	VkInstance instance = device.instance();
	VkSurfaceKHR surface;
	CHECK_VULKAN(glfwCreateWindowSurface(instance, window, nullptr, &surface));

	return {
		.device = device,
		.surface = surface,
		.surface_format = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		.present_mode = VK_PRESENT_MODE_FIFO_KHR,
		.frame_count = 2
	};
}

PipelineBuilder get_example_pipeline_builder(ExamplePipelineInfo example, VulkanDevice device, VulkanSwapchain swapchain)
{
	ShaderBuilder example_vertex_builder = {
		.device = device,
		.code = read_file_into_buffer("shader/example.vert.spv"),
		.entry_point = "main"
	};

	ShaderBuilder example_fragment_builder = {
		.device = device,
		.code = read_file_into_buffer("shader/example.frag.spv"),
		.entry_point = "main"
	};

	return {
		.device = device,
		.renderpass = example.get_renderpass(device, swapchain),
		.vertex_shader = VulkanShader(example_vertex_builder),
		.tesselation = {},
		.geometry_shader = {},
		.fragment_shader = VulkanShader(example_fragment_builder),
		.dynamic_state = example.get_dynamic_state(),
		.vertex_input = example.get_vertex_input(),
		.input_assembly = example.get_input_assembly(),
		.resterization = example.get_resterization(),
		.color_blend = example.get_color_blend(),
		.viewport = example.get_viewport(),
		.multisample = example.get_multisample()
	};
}

int main()
{
	glfwInit();
	GLFWwindow* window = create_default_window(1024, 768);
	VulkanDevice device = {get_device_builder()};
	VulkanSwapchain swapchain = {get_swapchain_builder(device, window)};
	ExamplePipelineInfo example;
	VulkanPipeline pipeline = {get_example_pipeline_builder(example, device, swapchain)};
	glfwTerminate();
	return 0;
}
