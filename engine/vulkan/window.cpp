#include "engine/vulkan/window.h"
#include "engine/vulkan/debug.h"
#include <chrono>

namespace kodanuki
{

static int window_counter = 0;

GLFWwindow* create_window(WindowBuilder builder)
{
	if (window_counter == 0) {
		glfwInit();
	}
	window_counter++;

	auto monitor = builder.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
	int width = builder.fullscreen ? glfwGetVideoMode(monitor)->width : builder.width;
	int height = builder.fullscreen ? glfwGetVideoMode(monitor)->height : builder.height;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, builder.resizeable ? GLFW_TRUE : GLFW_FALSE);
	auto window = glfwCreateWindow(width, height, builder.title.c_str(), monitor, NULL);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	return window;
}

struct WindowState
{
	GLFWwindow* window;
	float delta_time;
	std::chrono::high_resolution_clock::time_point current_time;
	~WindowState();
};

WindowState::~WindowState()
{
	glfwDestroyWindow(window);

	window_counter--;
	if (window_counter == 0) {
		glfwTerminate();
	}
}

VulkanWindow::VulkanWindow(WindowBuilder builder)
{
	auto window = create_window(builder);
	state = std::make_shared<WindowState>(window);
	state->current_time = std::chrono::high_resolution_clock::now();
}

VulkanWindow::operator GLFWwindow*()
{
	return state->window;
}

bool VulkanWindow::tick()
{
	auto new_time = std::chrono::high_resolution_clock::now();
	state->delta_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - state->current_time).count();
	state->current_time = new_time;
	return !should_close();
}

VkSurfaceKHR VulkanWindow::create_surface(VulkanDevice device)
{
	VkInstance instance = device.instance();
	VkSurfaceKHR surface;
	CHECK_VULKAN(glfwCreateWindowSurface(instance, state->window, nullptr, &surface));
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
	glfwPollEvents();
	return glfwWindowShouldClose(state->window);
}

float VulkanWindow::get_delta_time_seconds()
{
	return state->delta_time;
}

bool VulkanWindow::is_key_pressed(int key, int action)
{
	return glfwGetKey(state->window, key) == action;
}

};
