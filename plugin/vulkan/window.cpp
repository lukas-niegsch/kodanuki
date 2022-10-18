#include "plugin/vulkan/window.h"
#include "plugin/vulkan/debug.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Kodanuki
{

static int window_counter = 0;

GLFWwindow* create_window(WindowBuilder builder)
{
	GLFWwindow* window;
	VkExtent2D size = builder.default_size;
   	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, builder.resizeable ? GLFW_TRUE : GLFW_FALSE);
	window = glfwCreateWindow(size.width, size.height, builder.title.c_str(), NULL, NULL);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	return window;
}

void remove_window(Entity* window)
{
	GLFWwindow* glfw_window = ECS::get<GLFWwindow*>(*window);
	glfwDestroyWindow(glfw_window);
	ECS::remove<Entity>(*window);
	delete window;

	window_counter--;
	if (window_counter == 0) {
		glfwTerminate();
	}
}

VulkanWindow::VulkanWindow(WindowBuilder builder)
{
	pimpl = std::shared_ptr<Entity>(new Entity, &remove_window);
	Entity window = *pimpl = ECS::create();
	
	if (window_counter == 0) {
		glfwInit();
	}
	window_counter++;

	ECS::update<GLFWwindow*>(window, create_window(builder));
}

VkSurfaceKHR VulkanWindow::create_surface(VulkanDevice device)
{
	GLFWwindow* window = ECS::get<GLFWwindow*>(*pimpl);
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

};
