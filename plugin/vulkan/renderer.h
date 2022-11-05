#pragma once
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/pipeline.h"
#include "plugin/vulkan/renderpass.h"
#include "engine/central/entity.h"
#include "engine/template/copyable.h"
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
class VulkanRenderer : public Copyable<VulkanRenderer>
{
public:
	// Creates a new vulkan renderer from the given builder.
	VulkanRenderer(RendererBuilder builder);

	// Called automatically once all instances are unused.
	void shared_destructor();

public:
	// Aquires the next frame from the swapchain.
	void aquire();

	// Submits all the draww commands to the device.
	void submit(uint32_t queue_index = 0);

	// Enques the model for drawing to the current frame.
	// TODO: replace pipeline with model and instance data
	void draw(VulkanPipeline model);

	// Records the enqueued models onto frame buffers.
	void record();

	// Renders the next available frame onto the screen.
	void render(uint32_t queue_index = 0);
};

}
