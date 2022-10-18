#pragma once
#include "plugin/vulkan/device.h"
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Kodanuki
{

/**
 * Contains all the configurable information for creating a renderpass.
 *
 * These values will be considered as much as possible when creating
 * the renderpass. The caller has to make sure that the provided values
 * make sense.
 *
 * Example:
 * The device must match the device of the pipeline.
 * The attachments must match the shaders.
 */
struct RenderpassBuilder
{
	// The vulkan device for which the renderpass is created.
	VulkanDevice device;

	// The list of subpass dependencies.
	std::vector<VkSubpassDependency> dependencies;

	// The list of the attachment descriptions.
	std::vector<VkAttachmentDescription> attachments;

	// The list of the render subpasses.
	std::vector<VkSubpassDescription> subpasses;
};

/**
 * The vulkan renderer is a wrapper around the renderpass.
 * 
 * Instances can be copied around freely and will release all
 * ressources once unused.
 */
class VulkanRenderpass
{
public:
	// Creates a new vulkan rendererpass from the given builder.
	VulkanRenderpass(RenderpassBuilder builder);

public:
	// Returns the handle to the actual renderpass.
	VkRenderPass renderpass();

private:
	// Destroys unused rendererpasses automatically.
	std::shared_ptr<Entity> pimpl;
};

}
