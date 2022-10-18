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

struct Synchronization
{
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> draw_frame_fences;
	uint32_t current_frame;
};

Synchronization create_synchronization(RendererBuilder builder)
{
	VkDevice device = builder.device.logical_device();
	uint32_t frame_count = builder.swapchain.image_views().size();

	Synchronization synchronization;
	synchronization.image_available_semaphores.resize(frame_count);
	synchronization.render_finished_semaphores.resize(frame_count);
	synchronization.draw_frame_fences.resize(frame_count);
	synchronization.current_frame = 0;

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < frame_count; i++) {
		CHECK_VULKAN(vkCreateSemaphore(device, &semaphore_info, nullptr, &synchronization.image_available_semaphores[i]));
		CHECK_VULKAN(vkCreateSemaphore(device, &semaphore_info, nullptr, &synchronization.render_finished_semaphores[i]));
		CHECK_VULKAN(vkCreateFence(device, &fence_info, nullptr, &synchronization.draw_frame_fences[i]));
	}

	return synchronization;
}

void remove_renderer(Entity* renderer)
{
	VkDevice device = ECS::get<VulkanDevice>(*renderer).logical_device();
	std::vector<VkFramebuffer> frame_buffers = ECS::get<std::vector<VkFramebuffer>>(*renderer);
	Synchronization synchronization = ECS::get<Synchronization>(*renderer);
	for (VkSemaphore semaphore : synchronization.image_available_semaphores) {
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	for (VkSemaphore semaphore : synchronization.render_finished_semaphores) {
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	for (VkFence fence : synchronization.draw_frame_fences) {
		vkDestroyFence(device, fence, nullptr);
	}
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
	Synchronization synchronization = create_synchronization(builder);

	ECS::update<VulkanDevice>(renderer, builder.device);
	ECS::update<VulkanSwapchain>(renderer, builder.swapchain);
	ECS::update<VulkanRenderpass>(renderer, builder.renderpass);
	ECS::update<std::vector<VkFramebuffer>>(renderer, frame_buffers);
	ECS::update<std::vector<VkCommandBuffer>>(renderer, command_buffers);
	ECS::update<Synchronization>(renderer, synchronization);
	ECS::update<VkClearValue>(renderer, builder.clear_color);
}

void VulkanRenderer::aquire_next_frame()
{

}

void VulkanRenderer::record_command_buffer(std::function<void(VkCommandBuffer)> callback, uint32_t buffer_index)
{
	(void) callback;
	(void) buffer_index;
}

void VulkanRenderer::submit_command_buffers()
{

}

void VulkanRenderer::render_next_frame()
{

}

}
