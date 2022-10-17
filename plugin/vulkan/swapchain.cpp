#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/debug.h"

namespace Kodanuki
{

VkSwapchainKHR create_swapchain(SwapchainCreateInfo builder)
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(builder.device.physical_device(), builder.surface, &capabilities);

	VkSwapchainCreateInfoKHR swapchain_info = {};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.surface = builder.surface;
	swapchain_info.minImageCount = builder.frame_count;
	swapchain_info.imageFormat = builder.surface_format.format;
	swapchain_info.imageColorSpace = builder.surface_format.colorSpace;
	swapchain_info.imageExtent = capabilities.currentExtent;
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_info.queueFamilyIndexCount = 0;
	swapchain_info.pQueueFamilyIndices = nullptr;
	swapchain_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_info.presentMode = builder.present_mode;
	swapchain_info.clipped = true;
	swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR result;
	CHECK_VULKAN(vkCreateSwapchainKHR(builder.device.logical_device(), &swapchain_info, nullptr, &result));
	return result;
}

std::vector<VkImageView> create_image_views(SwapchainCreateInfo builder, VkSwapchainKHR swapchain)
{
	VkDevice logical_device = builder.device.logical_device();
	std::vector<VkImage> images = vectorize<vkGetSwapchainImagesKHR>(logical_device, swapchain);
	std::vector<VkImageView> views(images.size());

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = builder.surface_format.format;
	view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	for (uint32_t i = 0; i < views.size(); i++)
	{
		view_info.image = images[i];
		CHECK_VULKAN(vkCreateImageView(logical_device, &view_info, nullptr, &views[i]));
	}

	return views;
}

void remove_swapchain(Entity* swapchain)
{
	VulkanDevice device = ECS::get<VulkanDevice>(*swapchain);
	VkSurfaceKHR surface = ECS::get<VkSurfaceKHR>(*swapchain);
	VkSwapchainKHR actual_swapchain = ECS::get<VkSwapchainKHR>(*swapchain);
	std::vector<VkImageView> views = ECS::get<std::vector<VkImageView>>(*swapchain);
	
	for (auto view : views) {
		vkDestroyImageView(device.logical_device(), view, nullptr);
	}
	vkDestroySwapchainKHR(device.logical_device(), actual_swapchain, nullptr);
	vkDestroySurfaceKHR(device.instance(), surface, nullptr);
	ECS::remove<Entity>(*swapchain);
	delete swapchain;
}

VulkanSwapchain::VulkanSwapchain(SwapchainCreateInfo builder)
{
	pimpl = std::shared_ptr<Entity>(new Entity, &remove_swapchain);
	Entity swapchain = *pimpl = ECS::create();

	VkSwapchainKHR actual_swapchain = create_swapchain(builder);
	std::vector<VkImageView> views = create_image_views(builder, actual_swapchain);

	ECS::update<VulkanDevice>(swapchain, builder.device);
	ECS::update<VkSurfaceKHR>(swapchain, builder.surface);
	ECS::update<VkSwapchainKHR>(swapchain, actual_swapchain);
	ECS::update<std::vector<VkImageView>>(swapchain, views);
}

VkSurfaceKHR VulkanSwapchain::surface()
{
	return ECS::get<VkSurfaceKHR>(*pimpl);
}

std::vector<VkImageView> VulkanSwapchain::image_views()
{
	return ECS::get<std::vector<VkImageView>>(*pimpl);
}

}
