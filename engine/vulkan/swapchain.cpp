#include "engine/vulkan/swapchain.h"
#include "engine/vulkan/debug.h"

namespace Kodanuki
{

VulkanSwapchain::VulkanSwapchain(SwapchainBuilder builder)
{
	ECS::update<VulkanDevice>(impl, builder.device);
	ECS::update<VulkanRenderpass>(impl, builder.renderpass);
	ECS::update<VkSurfaceKHR>(impl, builder.surface);
	ECS::update<VkSurfaceFormatKHR>(impl, builder.surface_format);
	ECS::update<VkPresentModeKHR>(impl, builder.present_mode);
	ECS::update<VkSwapchainKHR>(impl, create_swapchain(builder.frame_count));
	ECS::update<std::vector<VkImageView>>(impl, create_image_views());
	ECS::update<std::vector<VkFramebuffer>>(impl, create_frame_buffers());
}

void VulkanSwapchain::shared_destructor()
{
	VulkanDevice device = ECS::get<VulkanDevice>(impl);
	cleanup(device);
	vkDestroySurfaceKHR(device.instance(), ECS::get<VkSurfaceKHR>(impl), nullptr);
}

VulkanSwapchain::operator VkSwapchainKHR()
{
	return ECS::get<VkSwapchainKHR>(impl);
}

void VulkanSwapchain::recreate_swapchain()
{
	VkDevice device = ECS::get<VulkanDevice>(impl);
	uint32_t count = frame_count();
	vkDeviceWaitIdle(device);
	cleanup(device);
	ECS::update<VkSwapchainKHR>(impl, create_swapchain(count));
	ECS::update<std::vector<VkImageView>>(impl, create_image_views());
	ECS::update<std::vector<VkFramebuffer>>(impl, create_frame_buffers());
}

VkExtent2D VulkanSwapchain::surface_extent()
{
	VkPhysicalDevice device = ECS::get<VulkanDevice>(impl).physical_device();
	VkSurfaceKHR surface = ECS::get<VkSurfaceKHR>(impl);
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);
	return capabilities.currentExtent;
}

uint32_t VulkanSwapchain::frame_count()
{
	return ECS::get<std::vector<VkFramebuffer>>(impl).size();
}

std::vector<VkFramebuffer> VulkanSwapchain::frame_buffers()
{
	return ECS::get<std::vector<VkFramebuffer>>(impl);
}

void VulkanSwapchain::cleanup(VkDevice device)
{
	for (VkFramebuffer buffer : ECS::get<std::vector<VkFramebuffer>>(impl)) {
		vkDestroyFramebuffer(device, buffer, nullptr);
	}
	for (VkImageView view : ECS::get<std::vector<VkImageView>>(impl)) {
		vkDestroyImageView(device, view, nullptr);
	}
	vkDestroySwapchainKHR(device, ECS::get<VkSwapchainKHR>(impl), nullptr);
}

VkSwapchainKHR VulkanSwapchain::create_swapchain(uint32_t frame_count)
{
	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.surface = ECS::get<VkSurfaceKHR>(impl);
	info.minImageCount = frame_count;
	info.imageFormat = ECS::get<VkSurfaceFormatKHR>(impl).format;
	info.imageColorSpace = ECS::get<VkSurfaceFormatKHR>(impl).colorSpace;
	info.imageExtent = surface_extent();
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount = 0;
	info.pQueueFamilyIndices = nullptr;
	info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode = ECS::get<VkPresentModeKHR>(impl);
	info.clipped = true;
	info.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR result;
	CHECK_VULKAN(vkCreateSwapchainKHR(ECS::get<VulkanDevice>(impl), &info, nullptr, &result));
	return result;
}

std::vector<VkImageView> VulkanSwapchain::create_image_views()
{
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = ECS::get<VkSurfaceFormatKHR>(impl).format;
	info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;

	VkDevice device = ECS::get<VulkanDevice>(impl);
	std::vector<VkImage> images = vectorize<vkGetSwapchainImagesKHR>(device, *(this));
	std::vector<VkImageView> views(images.size());
	for (uint32_t i = 0; i < views.size(); i++) {
		info.image = images[i];
		CHECK_VULKAN(vkCreateImageView(device, &info, nullptr, &views[i]));
	}
	return views;
}

std::vector<VkFramebuffer> VulkanSwapchain::create_frame_buffers()
{
	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = ECS::get<VulkanRenderpass>(impl);
	info.layers = 1;
	info.width = surface_extent().width;
	info.height = surface_extent().height;

	VkDevice device = ECS::get<VulkanDevice>(impl);
	std::vector<VkImageView> views = ECS::get<std::vector<VkImageView>>(impl);
	std::vector<VkFramebuffer> framebuffers(views.size());	
	for (uint32_t i = 0; i < framebuffers.size(); i++) {
		std::vector<VkImageView> attachments = {views[i]};
		info.attachmentCount = attachments.size();
		info.pAttachments = attachments.data();
		CHECK_VULKAN(vkCreateFramebuffer(device, &info, nullptr, &framebuffers[i]));
	}
	return framebuffers;
}

}
