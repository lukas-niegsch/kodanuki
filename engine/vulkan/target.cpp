#include "engine/vulkan/target.h"
#include "engine/vulkan/debug.h"
#include "engine/vulkan/wrapper.h"

namespace kodanuki
{

struct TargetState
{
public:
	VulkanDevice device;
	VulkanWindow window;
	VkRenderPass renderpass;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	VkSurfaceFormatKHR surface_format;
	VkPresentModeKHR present_mode;
	VkExtent2D surface_extent;
	uint32_t frame_count;
	std::vector<VkImageView> imageviews;
	std::vector<VkFramebuffer> framebuffers;
	VkFormat depth_image_format;
	VkImage depth_image;
	Wrapper<VkDeviceMemory> depth_image_memory;
	Wrapper<VkImageView> depth_image_view;

public:
	~TargetState();
	void create_surface();
	void delete_surface();
	void create_renderpass();
	void delete_renderpass();
	void create_swapchain();
	void delete_swapchain();
	void create_imageviews();
	void delete_imageviews();
	void create_framebuffers();
	void delete_framebuffers();
	void create_depth_image();
	void delete_depth_image();
};

TargetState::~TargetState()
{
	delete_framebuffers();
	delete_imageviews();
	delete_swapchain();
	delete_surface();
	delete_depth_image();
	vkDestroyRenderPass(device, renderpass, nullptr);
}

void TargetState::create_surface()
{
	surface = window.create_surface(device);
}

void TargetState::delete_surface()
{
	vkDestroySurfaceKHR(device.get_instance(), surface, nullptr);
}

void TargetState::create_swapchain()
{
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
	CHECK_VULKAN(vkCreateSwapchainKHR(device, &info, nullptr, &swapchain));
}

void TargetState::delete_swapchain()
{
	vkDeviceWaitIdle(device);
	vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void TargetState::create_imageviews()
{
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = surface_format.format;
	info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;

	std::vector<VkImage> images = vectorize<vkGetSwapchainImagesKHR>(device, swapchain);
	imageviews.resize(images.size());

	for (uint32_t i = 0; i < imageviews.size(); i++) {
		info.image = images[i];
		CHECK_VULKAN(vkCreateImageView(device, &info, nullptr, &imageviews[i]));
	}
}

void TargetState::delete_imageviews()
{
	vkDeviceWaitIdle(device);
	for (VkImageView view : imageviews) {
		vkDestroyImageView(device, view, nullptr);
	}
}

void TargetState::create_framebuffers()
{
	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = renderpass;
	info.layers = 1;
	info.width = surface_extent.width;
	info.height = surface_extent.height;

	framebuffers.resize(imageviews.size());

	for (uint32_t i = 0; i < framebuffers.size(); i++) {
		std::vector<VkImageView> attachments = {imageviews[i], depth_image_view};
		info.attachmentCount = attachments.size();
		info.pAttachments = attachments.data();
		CHECK_VULKAN(vkCreateFramebuffer(device, &info, nullptr, &framebuffers[i]));
	}
}

void TargetState::delete_framebuffers()
{
	vkDeviceWaitIdle(device);
	for (VkFramebuffer buffer : framebuffers) {
		vkDestroyFramebuffer(device, buffer, nullptr);
	}
}

void TargetState::create_depth_image()
{
	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = surface_extent.width;
	image_info.extent.height = surface_extent.height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.format = depth_image_format;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	CHECK_VULKAN(vkCreateImage(device, &image_info, nullptr, &depth_image));

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, depth_image, &memory_requirements);

	depth_image_memory = create_device_memory(device, device.get_physical_device(),
		memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkBindImageMemory(device, depth_image, depth_image_memory, 0);
	depth_image_view = create_image_view(device, depth_image_format, depth_image,
		VK_IMAGE_ASPECT_DEPTH_BIT);
}

void TargetState::delete_depth_image()
{
	depth_image_view = {};
	vkDestroyImage(device, depth_image, nullptr);
	depth_image_memory = {};
}

VulkanTarget::VulkanTarget(TargetBuilder builder)
{
	state = std::make_shared<TargetState>(builder.device, builder.window);
	builder.create_renderpass(state->device, state->renderpass);
	state->surface_format = builder.surface_format;
	state->present_mode = builder.present_mode;
	state->frame_count = builder.frame_count;
	state->create_surface();
	state->surface_extent = get_surface_extent();
	state->create_swapchain();
	state->create_imageviews();
	state->depth_image_format = builder.depth_image_format;
	state->create_depth_image();
	state->create_framebuffers();
}

VkRenderPass VulkanTarget::renderpass()
{
	return state->renderpass;
}

VkSwapchainKHR VulkanTarget::swapchain()
{
	return state->swapchain;
}

VkExtent2D VulkanTarget::get_surface_extent()
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(state->device.get_physical_device(), state->surface, &capabilities);
	return capabilities.currentExtent;
}

uint32_t VulkanTarget::get_frame_count()
{
	return state->frame_count;
}

VkFramebuffer VulkanTarget::get_frame_buffer(uint32_t index)
{
	return state->framebuffers[index % state->frame_count];
}

void VulkanTarget::recreate_swapchain()
{
	CHECK_VULKAN(vkDeviceWaitIdle(state->device));
	state->surface_extent = get_surface_extent();
	state->delete_imageviews();
	state->delete_framebuffers();
	state->delete_depth_image();
	state->create_swapchain();
	state->create_imageviews();
	state->create_depth_image();
	state->create_framebuffers();
}

};
