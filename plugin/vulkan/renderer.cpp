#include "plugin/vulkan/renderer.h"
#include "plugin/vulkan/debug.h"

namespace Kodanuki
{

VkExtent2D get_surface_extent(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);
	return capabilities.currentExtent;
}

std::vector<VkFramebuffer> create_frame_buffers(RendererBuilder builder)
{
	std::vector<VkImageView> views = builder.swapchain.image_views();
	std::vector<VkFramebuffer> framebuffers(views.size());
	VkDevice logical_device = builder.device.logical_device();
	VkPhysicalDevice physical_device = builder.device.physical_device();
	VkExtent2D size = get_surface_extent(physical_device, builder.swapchain.surface());

	VkFramebufferCreateInfo framebuffer_info = {};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.renderPass = builder.renderpass.renderpass();
	framebuffer_info.layers = 1;
	framebuffer_info.width = size.width;
	framebuffer_info.height = size.height;

	for (uint32_t i = 0; i < framebuffers.size(); i++) {
		std::vector<VkImageView> attachments = {views[i]};
		framebuffer_info.attachmentCount = attachments.size();
		framebuffer_info.pAttachments = attachments.data();

		CHECK_VULKAN(vkCreateFramebuffer(logical_device, &framebuffer_info, nullptr, &framebuffers[i]));
	}

	return framebuffers;
}

std::vector<VkCommandBuffer> create_command_buffers(RendererBuilder builder)
{
	VkCommandBufferAllocateInfo buffer_info;
    buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_info.pNext = nullptr;
    buffer_info.commandPool = builder.device.command_pool();
    buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_info.commandBufferCount = builder.command_buffer_count;

	VkDevice device = builder.device.logical_device();
	std::vector<VkCommandBuffer> command_buffers(builder.command_buffer_count);
	CHECK_VULKAN(vkAllocateCommandBuffers(device, &buffer_info, command_buffers.data()));
	return command_buffers;
}

void remove_renderer(Entity* renderer)
{
	VkDevice device = ECS::get<VulkanDevice>(*renderer).logical_device();
	std::vector<VkFramebuffer> frame_buffers = ECS::get<std::vector<VkFramebuffer>>(*renderer);
	for (auto frame_buffer : frame_buffers) {
		vkDestroyFramebuffer(device, frame_buffer, nullptr);
	}
	ECS::remove<Entity>(*renderer);
	delete renderer;
}

VulkanRenderer::VulkanRenderer(RendererBuilder builder)
{
	pimpl = std::shared_ptr<Entity>(new Entity, &remove_renderer);
	Entity renderer = *pimpl = ECS::create();

	std::vector<VkFramebuffer> frame_buffers = create_frame_buffers(builder);
	std::vector<VkCommandBuffer> command_buffers = create_command_buffers(builder);

	ECS::update<VulkanDevice>(renderer, builder.device);
	ECS::update<VulkanSwapchain>(renderer, builder.swapchain);
	ECS::update<VulkanRenderpass>(renderer, builder.renderpass);
	ECS::update<std::vector<VkFramebuffer>>(renderer, frame_buffers);
	ECS::update<std::vector<VkCommandBuffer>>(renderer, command_buffers);
}

}
