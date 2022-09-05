#include "engine/graphics/window.h"

namespace Kodanuki
{

WindowModule::WindowModule(GameInfo& info, VkExtent2D extent)
	: info(info), defaultExtent(extent) {}

void WindowModule::attach(Family)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	int width = defaultExtent.width;
	int height = defaultExtent.height;
	window = glfwCreateWindow(width, height, info.title, nullptr, nullptr);
}

void WindowModule::detach(Family)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void WindowModule::update(Family)
{
	if (glfwWindowShouldClose(window))
	{
		info.running = false;
	}
	glfwPollEvents();
}

VkSurfaceKHR WindowModule::createSurface(VkInstance instance)
{
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(instance, window, nullptr, &surface);
	return surface;
}

void WindowModule::resetExtent()
{
	setExtent(defaultExtent);
}

void WindowModule::setExtent(VkExtent2D extent)
{
	glfwSetWindowSize(window, extent.width, extent.height);
}

VkExtent2D WindowModule::getExtent()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

}
