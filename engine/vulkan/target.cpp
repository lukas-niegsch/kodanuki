#include "engine/vulkan/target.h"
#include "engine/vulkan/debug.h"
#include "engine/vulkan/wrapper.h"

namespace kodanuki
{

VulkanTarget::VulkanTarget(TargetBuilder builder)
: device(builder.device)
, window(builder.window)
{
	depth_image_format = builder.depth_image_format;
	surface_format = builder.surface_format;
	present_mode = builder.present_mode;
	frame_count = builder.frame_count;
	renderpass = create_renderpass(device, builder.create_renderpass);
	surface = create_surface(device.get_instance(), window);
	update_target_swapchain();
}

void VulkanTarget::update_target_swapchain()
{
	surface_extent = get_surface_extent();
	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.surface = surface;
	info.minImageCount = frame_count;
	info.imageFormat = surface_format.format;
	info.imageColorSpace = surface_format.colorSpace;
	info.imageExtent = surface_extent;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount = 0;
	info.pQueueFamilyIndices = nullptr;
	info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode = present_mode;
	info.clipped = true;
	info.oldSwapchain = swapchain;
	swapchain = create_swapchain(device, info);
	update_depth_image();
	update_render_images();
}

void VulkanTarget::update_render_images()
{
	imageviews.clear();
	framebuffers.clear();
	for (VkImage image : vectorize<vkGetSwapchainImagesKHR>(device, swapchain)) {
		Wrapper<VkImageView> view = create_image_view(device,
			surface_format.format, image, VK_IMAGE_ASPECT_COLOR_BIT);
		imageviews.push_back(view);
		Wrapper<VkFramebuffer> buffer = create_frame_buffer(device,
			renderpass, surface_extent, {view, depth_image_view});
		framebuffers.push_back(buffer);
	}
}

void VulkanTarget::update_depth_image()
{
	VkImageCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = VK_IMAGE_TYPE_2D;
	info.extent.width = surface_extent.width;
	info.extent.height = surface_extent.height;
	info.extent.depth = 1;
	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.format = depth_image_format;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	depth_image = create_image(device, info);

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, depth_image, &memory_requirements);

	depth_image_memory = create_device_memory(device, device.get_physical_device(),
		memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkBindImageMemory(device, depth_image, depth_image_memory, 0);
	depth_image_view = create_image_view(device, depth_image_format, depth_image,
		VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkRenderPass VulkanTarget::get_renderpass()
{
	return renderpass;
}

VkSwapchainKHR VulkanTarget::get_swapchain()
{
	return swapchain;
}

VkExtent2D VulkanTarget::get_surface_extent()
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.get_physical_device(), surface, &capabilities);
	return capabilities.currentExtent;
}

uint32_t VulkanTarget::get_frame_count()
{
	return frame_count;
}

VkFramebuffer VulkanTarget::get_frame_buffer(uint32_t index)
{
	return framebuffers[index % frame_count];
}

};
