#pragma once
#include "engine/vulkan/device.h"
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace kodanuki
{

/**
 * The parameters that are used to build a new vulkan window.
 *
 * Some parameters might be ignored depending on the input. For example
 * the width, height and title while in fullscreen mode. The user should
 * make sure that these values make sense, for example that the width is
 * not to high.
 */
struct WindowBuilder
{
	// The title of the window.
    std::string title;

	// The default width of the window.
    uint64_t width;

	// The default height of the window.
    uint64_t height;

	// Flag to make the window resizeable.
    bool resizeable;

	// Flag to make the window fullscreen.
    bool fullscreen;

	// The preferred number of frames per second.
    uint64_t frames;
};

/**
 * The vulkan window is a wrapper around the glfw window.
 * 
 * Each vulkan window creates an glfw window and handle key events
 * and other window events.
 *
 * Instances can be copied around freely and will release all
 * ressources once unused.
 */
class VulkanWindow
{
public:
	// Creates a new vulkan window from the given builder.
	VulkanWindow(WindowBuilder builder);

	// Returns the handle to the glfw window.
	operator GLFWwindow*();

	// Sleeps until the game loop should run again.
	bool tick();

public:
	void set_cursor_movement_callback(std::function<void(float, float)> callback);
	void set_cursor_scroll_callback(std::function<void(float)> callback);

public:
	// Returns the handle to the surface.
	VkSurfaceKHR create_surface(VulkanDevice device);

	// Returns the list of required instance extensions.
	std::vector<const char*> required_instance_extensions();

	// Returns true iff the window should close.
	bool should_close();

	// Returns the delta time that passed between ticks.
	float get_delta_time_seconds();

	// Returns true iff the key is being pressed.
	bool is_key_pressed(int key, int action = GLFW_PRESS);

	// Returns true iff the mouse button is being pressed.
	bool is_mouse_button_pressed(int key, int action = GLFW_PRESS);

private:
	// The abstract pointer to the implementation.
	std::shared_ptr<struct WindowState> state;
};

}
