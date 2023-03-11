#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/window.h"
#include <vulkan/vulkan.h>
#include <functional>
#include <memory>
#include <vector>

namespace kodanuki
{

/**
 * Contains all the configurable information for creating a target.
 *
 * The caller has to make sure that the provided values make sense.
 * To get more debug information enable vulkan validation layers.
 *
 * Example:
 * The surface must support the surface format and the number of
 * frame buffers. The present mode must also be supported by the GPU.
 */
struct TargetBuilder
{
	// The vulkan device which does the rendering.
	VulkanDevice device;

    // The vulkan window on which the viewport renders.
    VulkanWindow window;

    // The function that creates the renderpass.
    std::function<void(VulkanDevice, VkRenderPass&)> create_renderpass;

	// The format of the surface.
	VkSurfaceFormatKHR surface_format;

	// The format of the depth image.
	VkFormat depth_image_format;

	// The mode for swapping frames.
	VkPresentModeKHR present_mode;

	// The number of frame buffers.
	uint32_t frame_count;
};

/**
 * The vulkan target combines the swapchain and the renderpass.
 *
 * The target is dynamic and will react to changes from the
 * used window, like resizing and minimizing. The size should
 * be changed via the window. This will update the surface
 * via callbacks.
 *
 * Instances can be copied around freely and will release all
 * ressources once unused.
 */
class VulkanTarget
{
public:
    // Creates a new vulkan target from the given builder.
    VulkanTarget(TargetBuilder builder);

public:
    // Returns the handle to the created renderpass.
    VkRenderPass renderpass();

	// Returns the handle to the created swapchain.
	VkSwapchainKHR swapchain();

    // Returns the current extent of the surface.
    VkExtent2D get_surface_extent();

	// Returns the frame count.
	uint32_t get_frame_count();

	// Returns the handle to frame buffer at the given index.
	VkFramebuffer get_frame_buffer(uint32_t index);

	// Recreates the swapchain, must be called on overy surface change.
	void recreate_swapchain();

private:
	// The abstract pointer to the implementation.
	std::shared_ptr<struct TargetState> state;
};

}
