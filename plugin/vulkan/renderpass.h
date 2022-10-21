#pragma once
#include "plugin/vulkan/device.h"
#include "engine/template/copyable.h"
#include <vulkan/vulkan.h>
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
class VulkanRenderpass : private CopyableWrapper<VulkanRenderpass>
{
public:
	// Creates a new vulkan rendererpass from the given builder.
	VulkanRenderpass(RenderpassBuilder builder);

public:
	// Returns the handle to the actual renderpass.
	VkRenderPass renderpass();

private:
	// Called once all renderpass copies are unused.
	void shared_destructor();
	friend class CopyableWrapper<VulkanRenderpass>;
};

}
