#include "engine/graphics/window.h"

namespace Kodanuki
{

WindowModule::WindowModule(GameInfo& info, VkExtent2D extend)
	: info(info), defaultExtend(extend) {}

void WindowModule::onAttach()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	int width = defaultExtend.width;
	int height = defaultExtend.height;
	window = glfwCreateWindow(width, height, info.title, nullptr, nullptr);
}

void WindowModule::onDetach()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void WindowModule::onUpdate(float)
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

void WindowModule::resetExtend()
{
	setExtend(defaultExtend);
}

void WindowModule::setExtend(VkExtent2D extend)
{
	glfwSetWindowSize(window, extend.width, extend.height);
}

VkExtent2D WindowModule::getExtend()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

}
