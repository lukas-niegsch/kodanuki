#pragma once
#include "plugin/vulkan/device.h"
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Kodanuki
{

struct RenderpassBuilder
{
	VulkanDevice device;
	std::vector<VkSubpassDependency> dependencies;
	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkSubpassDescription> subpasses;
};

class VulkanRenderpass
{
public:
	VulkanRenderpass(RenderpassBuilder builder);

public:
	VkRenderPass renderpass();

private:
	std::shared_ptr<Entity> pimpl;
};

}
