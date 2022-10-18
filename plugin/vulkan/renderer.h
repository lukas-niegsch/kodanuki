#pragma once
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/pipeline.h"
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

	// The vulkan pipeline for which the pipeline is created.
	VulkanPipeline pipeline;
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

private:
	// Destroys unused renderers automatically.
	std::shared_ptr<Entity> pimpl;
};

}
