#include "engine/vulkan/renderpass.h"
#include "engine/vulkan/debug.h"

namespace Kodanuki
{

VkRenderPass create_renderpass(RenderpassBuilder builder)
{
	VkRenderPassCreateInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_info.pNext = nullptr;
	renderpass_info.flags = 0;
	renderpass_info.attachmentCount = builder.attachments.size();
	renderpass_info.pAttachments = builder.attachments.data();
	renderpass_info.subpassCount = builder.subpasses.size();
	renderpass_info.pSubpasses = builder.subpasses.data();
	renderpass_info.dependencyCount = builder.dependencies.size();
	renderpass_info.pDependencies = builder.dependencies.data();

	VkDevice device = builder.device;
	VkRenderPass renderpass;
	CHECK_VULKAN(vkCreateRenderPass(device, &renderpass_info, nullptr, &renderpass));
	return renderpass;
}

VulkanRenderpass::VulkanRenderpass(RenderpassBuilder builder)
{
	ECS::update<VulkanDevice>(impl, builder.device);
	ECS::update<VkRenderPass>(impl, create_renderpass(builder));
}

void VulkanRenderpass::shared_destructor()
{
	VkDevice device = ECS::get<VulkanDevice>(impl);
	vkDestroyRenderPass(device, *this, nullptr);
}

VulkanRenderpass::operator VkRenderPass()
{
	return ECS::get<VkRenderPass>(impl);
}

}
