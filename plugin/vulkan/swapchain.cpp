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

void remove_swapchain(Entity* swapchain)
{
    VulkanDevice device = ECS::get<VulkanDevice>(*swapchain);
    VkSurfaceKHR surface = ECS::get<VkSurfaceKHR>(*swapchain);
    VkSwapchainKHR actual_swapchain = ECS::get<VkSwapchainKHR>(*swapchain);

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

    ECS::update<VulkanDevice>(swapchain, builder.device);
    ECS::update<VkSurfaceKHR>(swapchain, builder.surface);
    ECS::update<VkSwapchainKHR>(swapchain, actual_swapchain);
}

VkSurfaceKHR VulkanSwapchain::surface()
{
    return ECS::get<VkSurfaceKHR>(*pimpl);
}

}
