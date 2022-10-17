#include "plugin/vulkan/device.h"
#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/debug.h"
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
	glfwInit();
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

DeviceCreateInfo get_device_create_info()
{
	return {
		.instance_layers = {"VK_LAYER_KHRONOS_validation"},
		.instance_extensions = {"VK_KHR_surface", "VK_KHR_xcb_surface"},
		.device_extensions = {"VK_KHR_swapchain"},
		.gpu_score = &score_physical_device,
		.queue_score = &score_queue_family,
		.queue_priorities = {1.0f}
	};
}

SwapchainCreateInfo get_swapchain_create_info(VulkanDevice device, GLFWwindow* window)
{
	VkInstance instance = device.instance();
	VkSurfaceKHR surface;
	CHECK_VULKAN(glfwCreateWindowSurface(instance, window, nullptr, &surface));
	uint32_t width, height;
	glfwGetWindowSize(window, (int*) &width, (int*) &height);

	return {
		.device = device,
		.surface = surface,
		.format = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		.present_mode = VK_PRESENT_MODE_FIFO_KHR,
		.extent = {width, height},
		.frame_count = 2
	};
}

int main()
{
	GLFWwindow* window = create_default_window(1024, 768);
	VulkanDevice device = {get_device_create_info()};
	VulkanSwapchain swapchain = {get_swapchain_create_info(device, window)};
	glfwTerminate();
	return 0;
}
