#pragma once
#include "plugin/vulkan/device.h"
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <string>
#include <vector>

namespace Kodanuki
{

/**
 * Contains all the configurable information for creating a window.
 */
struct WindowBuilder
{
	// The title of the window.
	std::string title;

	// The default size of the window.
	VkExtent2D default_size;

	// Enables window resizing.
	bool resizeable;
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

public:
	// Returns the handle to the surface.
	VkSurfaceKHR create_surface(VulkanDevice device);

	// Returns the list of required instance extensions.
	std::vector<const char*> required_instance_extensions();

	// Returns true iff the window should close.
	bool should_close();

private:
	// Destroys unused windows automatically.
	std::shared_ptr<Entity> pimpl;
};

}
