#include "plugin/vulkan/window.h"
#include "plugin/vulkan/debug.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Kodanuki
{

static int window_counter = 0;

void VulkanWindow::shared_destructor()
{
	GLFWwindow* window = ECS::get<GLFWwindow*>(impl);
	glfwDestroyWindow(window);

	window_counter--;
	if (window_counter == 0) {
		glfwTerminate();
	}
}

GLFWwindow* create_window(WindowBuilder builder)
{
	GLFWwindow* window;
	VkExtent2D size = builder.default_size;
   	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, builder.resizeable ? GLFW_TRUE : GLFW_FALSE);
	window = glfwCreateWindow(size.width, size.height, builder.title.c_str(), NULL, NULL);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	return window;
}

VulkanWindow::VulkanWindow(WindowBuilder builder)
{
	if (window_counter == 0) {
		glfwInit();
	}
	window_counter++;

	ECS::update<GLFWwindow*>(impl, create_window(builder));
}

VkSurfaceKHR VulkanWindow::create_surface(VulkanDevice device)
{
	GLFWwindow* window = ECS::get<GLFWwindow*>(impl);
	VkInstance instance = device.instance();
	VkSurfaceKHR surface;
	CHECK_VULKAN(glfwCreateWindowSurface(instance, window, nullptr, &surface));
	return surface;
}

std::vector<const char*> VulkanWindow::required_instance_extensions()
{
	uint32_t count;
	const char** extensions = glfwGetRequiredInstanceExtensions(&count);
	return std::vector<const char*>(extensions, extensions + count);
}

bool VulkanWindow::should_close()
{
	GLFWwindow* window = ECS::get<GLFWwindow*>(impl);
	glfwPollEvents();
	return glfwWindowShouldClose(window);
}

};
