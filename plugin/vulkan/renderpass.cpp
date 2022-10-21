#include "plugin/vulkan/renderpass.h"
#include "plugin/vulkan/debug.h"

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

void remove_renderpass(Entity* renderpass)
{
	VkDevice device = ECS::get<VulkanDevice>(*renderpass);
	VkRenderPass actual_renderpass = ECS::get<VkRenderPass>(*renderpass);
	vkDestroyRenderPass(device, actual_renderpass, nullptr);
	ECS::remove<Entity>(*renderpass);
	delete renderpass;
}

VulkanRenderpass::VulkanRenderpass(RenderpassBuilder builder)
{
	pimpl = std::shared_ptr<Entity>(new Entity, &remove_renderpass);
	Entity renderpass = *pimpl = ECS::create();

	VkRenderPass actual_renderpass = create_renderpass(builder);

	ECS::update<VulkanDevice>(renderpass, builder.device);
	ECS::update<VkRenderPass>(renderpass, actual_renderpass);
}

VkRenderPass VulkanRenderpass::renderpass()
{
	return ECS::get<VkRenderPass>(*pimpl);
}

}
