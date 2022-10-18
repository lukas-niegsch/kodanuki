#pragma once
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/pipeline.h"
#include "plugin/vulkan/renderpass.h"
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Kodanuki
{

/**
 * Contains all the configurable information for creating a renderer.
 *
 * Each renderer only maintains a single pipeline and swapchain. They can
 * however be combined differently.
 */
struct RendererBuilder
{
	// The vulkan device for which the pipeline is created.
	VulkanDevice device;

	// The vulkan swapchain for which the pipeline is created.
	VulkanSwapchain swapchain;

	// The vulkan renderpass for which the pipeline is created.
	VulkanRenderpass renderpass;

	// The number of command buffers that are created.
	uint32_t command_buffer_count;

	// The color with which the frames should be cleared.
	VkClearValue clear_color;
};

/**
 * The vulkan renderer is a wrapper around the rendering process.
 *
 * Each vulkan renderer creates some framebuffers, and lets the user record
 * and submit commands to the pipeline. Finally users can present the next
 * image according to the swapchain.
 * 
 * Instances can be copied around freely and will release all
 * ressources once unused.
 */
class VulkanRenderer
{
public:
	// Creates a new vulkan renderer from the given builder.
	VulkanRenderer(RendererBuilder builder);

public:
	// Aquires the next frame for which work can be submitted.
	void aquire_next_frame();

	// Records the command buffer at the given index with draw commands.
	void record_command_buffer(std::function<void(VkCommandBuffer)> callback, uint32_t buffer_index = 0);

	// Submits the command buffers to the vulkan device asynchronously.
	void submit_command_buffers(uint32_t queue_index = 0);

	// Renders the next available frame to the surface.
	void render_next_frame(uint32_t queue_index = 0);

private:
	// Destroys unused renderers automatically.
	std::shared_ptr<Entity> pimpl;
};

}
