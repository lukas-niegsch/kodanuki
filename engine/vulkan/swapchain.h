#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/renderpass.h"
#include "engine/central/entity.h"
#include "engine/template/copyable.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Kodanuki
{

struct SwapchainBuilder
{
	// The device that computes graphics.
	VulkanDevice device;

	// The renderpass for the swapchain.
	VulkanRenderpass renderpass;

	// The surface on which is rendered.
	VkSurfaceKHR surface;

	// The format of the surface.
	VkSurfaceFormatKHR surface_format;

	// The mode for swapping frames.
	VkPresentModeKHR present_mode;

	// The number of frame buffers.
	uint32_t frame_count;
};

/**
 * The vulkan swapchain is a wrapper around frame buffering.
 *
 * This class creates a swapchain for the given surface parameters
 * and creates images and frame buffers. It handles the swapping of
 * the buffered images.
 *
 * Instances of this class can be copied around freely and will
 * release all ressources automatically once all instances are no
 * longer used.
 */
class VulkanSwapchain : public Copyable<VulkanSwapchain>
{
public:
	// Creates a new vulkan swapchain from the given builder.
	VulkanSwapchain(SwapchainBuilder builder);

	// Called automatically once all instances are unused.
	void shared_destructor();

	// Converts this class to the native vulkan swapchain.
	operator VkSwapchainKHR();

public:
	// Recreates the swapchain once outdated.
	void recreate_swapchain();

	// Returns the current surface extent.
	VkExtent2D surface_extent();

	// Returns the number of used frames.
	uint32_t frame_count();

	// Returns the used frame buffers.
	std::vector<VkFramebuffer> frame_buffers();

private:
	void cleanup(VkDevice device);
	VkSwapchainKHR create_swapchain(uint32_t frame_count);
	std::vector<VkFramebuffer> create_frame_buffers();
	std::vector<VkImageView> create_image_views();
};

}
