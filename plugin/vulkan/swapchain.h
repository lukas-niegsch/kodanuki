#pragma once
#include "plugin/vulkan/device.h"
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Kodanuki
{

/**
 * Contains all the configurable information for creating a swapchain.
 *
 * These values will be considered as much as possible when creating
 * the swapchain. The caller has to make sure that the provided values make
 * sense.
 *
 * Example:
 * The required extensions must be enabled inside the device.
 * The gpu has at least the selected amount of queue families.
 * The present mode must be available.
 */
struct SwapchainCreateInfo
{
	// The vulkan device for which the swapchain is created.
	VulkanDevice device;

	// The window surface on which the frame should be rendered.
	VkSurfaceKHR surface;

	// The surface format that the swapchain will use.
	VkSurfaceFormatKHR format;

	// The present mode in which the frames will be rendered.
	VkPresentModeKHR present_mode;

	// The extent of the given surface.
	VkExtent2D extent;

	// The number of frames that should be buffered.
	uint32_t frame_count;
};

/**
 * The vulkan swapchain is a wrapper around the frame buffering.
 *
 * Each vulkan swapchain creates a swapchain from the given builder
 * and handles the swapping of the buffered images. It handles the
 * output of some vulkan pipeline.
 *
 * Instances can be compied around freely and will release all
 * ressources once unused. The vulkan swapchain maintains the give
 * surface (including destroying it afterwards).
 */
class VulkanSwapchain
{
public:
	/**
	 * Creates a new vulkan swapchain from the given builder.
	 *
	 * @param builder The information on how to build the swapchain.
	 */
	VulkanSwapchain(SwapchainCreateInfo builder);

public:
	// Returns the handle to the surface.
	VkSurfaceKHR surface();

private:
	// Destroys unused swapchains automatically.
	std::shared_ptr<Entity> pimpl;
};

}