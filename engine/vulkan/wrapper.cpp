#include "engine/vulkan/wrapper.h"
#include "engine/vulkan/debug.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>

namespace kodanuki
{

VulkanInstance create_instance(std::vector<const char*> layers, std::vector<const char*> extensions)
{
	VkApplicationInfo info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = nullptr,
		.applicationVersion = VK_MAKE_VERSION(0, 0, 0),
		.pEngineName = "kodanuki",
		.engineVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = VK_API_VERSION_1_3
	};
	return create_wrapper<vkCreateInstance, vkDestroyInstance>({
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &info,
		.enabledLayerCount = static_cast<uint32_t>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data()
	});
}

VulkanDevice create_device(VkPhysicalDevice physical_device, uint32_t queue_family, std::vector<float> queue_priorities, std::vector<const char*> extensions)
{
	VkDeviceQueueCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = queue_family,
		.queueCount = static_cast<uint32_t>(queue_priorities.size()),
		.pQueuePriorities = queue_priorities.data(),
	};
	VkPhysicalDeviceFeatures features = {};
	VkDeviceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data(),
		.pEnabledFeatures = &features
	};
	VkDevice* output = new VkDevice();
	CHECK_VULKAN(vkCreateDevice(physical_device, &create_info, nullptr, output));
	auto destroy = [=](VkDevice* ptr) {
		CHECK_VULKAN(vkDeviceWaitIdle(*ptr));
		vkDestroyDevice(*ptr, nullptr);
	};
	return Wrapper<VkDevice>(output, destroy);
}

VulkanShaderModule create_shader_module(VkDevice device, std::vector<char> code)
{
	return create_wrapper<vkCreateShaderModule, vkDestroyShaderModule>({
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data())
	}, device);
}

VulkanFence create_fence(VkDevice device, VkFenceCreateFlagBits flags)
{
	return create_wrapper<vkCreateFence, vkDestroyFence>({
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = flags
	}, device);
}

VulkanSemaphore create_semaphore(VkDevice device)
{
	return create_wrapper<vkCreateSemaphore, vkDestroySemaphore>({
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	}, device);
}

VulkanDescriptorPool create_descriptor_pool(VkDevice device, const std::vector<VkDescriptorPoolSize> pool_sizes)
{
	return create_wrapper<vkCreateDescriptorPool, vkDestroyDescriptorPool>({
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = 10 * static_cast<uint32_t>(pool_sizes.size()),
		.poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
		.pPoolSizes = pool_sizes.data()
	}, device);
}

VulkanDescriptorSet create_descriptor_set(VkDevice device, VkDescriptorPool pool, const VkDescriptorSetLayout layout)
{
	VkDescriptorSet* output = new VkDescriptorSet();
	VkDescriptorSetAllocateInfo info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &layout
	};
	CHECK_VULKAN(vkAllocateDescriptorSets(device, &info, output));
	auto destroy = [=](VkDescriptorSet* ptr) {
		vkFreeDescriptorSets(device, pool, 1, ptr);
	};
	return Wrapper<VkDescriptorSet>(output, destroy);
}

VulkanCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index)
{
	return create_wrapper<vkCreateCommandPool, vkDestroyCommandPool>({
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queue_family_index
	}, device);
}

VulkanCommandBuffer create_command_buffer(VkDevice device, VkCommandPool pool)
{
	VkCommandBuffer* output = new VkCommandBuffer();
	VkCommandBufferAllocateInfo info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	CHECK_VULKAN(vkAllocateCommandBuffers(device, &info, output));
	auto destroy = [=](VkCommandBuffer* ptr) {
		vkFreeCommandBuffers(device, pool, 1, ptr);
	};
	return Wrapper<VkCommandBuffer>(output, destroy);
}

VulkanQueryPool create_query_pool(VkDevice device, uint32_t time_stamps)
{
	return create_wrapper<vkCreateQueryPool, vkDestroyQueryPool>({
		.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queryType = VK_QUERY_TYPE_TIMESTAMP,
		.queryCount = time_stamps,
		.pipelineStatistics = 0
	}, device);
}

VulkanFrameBuffer create_frame_buffer(VkDevice device, VkRenderPass renderpass, VkExtent2D extent, std::vector<VkImageView> attachments)
{
	return create_wrapper<vkCreateFramebuffer, vkDestroyFramebuffer>({
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = renderpass,
		.attachmentCount = static_cast<uint32_t>(attachments.size()),
		.pAttachments = attachments.data(),
		.width = extent.width,
		.height = extent.height,
		.layers = 1
	}, device);
}

VulkanImageView create_image_view(VkDevice device, VkFormat format, VkImage image, VkImageAspectFlagBits mask)
{
	VkComponentMapping components = {
		.r = VK_COMPONENT_SWIZZLE_IDENTITY,
		.g = VK_COMPONENT_SWIZZLE_IDENTITY,
		.b = VK_COMPONENT_SWIZZLE_IDENTITY,
		.a = VK_COMPONENT_SWIZZLE_IDENTITY
	};
	VkImageSubresourceRange subresource_range = {
		.aspectMask = mask,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	return create_wrapper<vkCreateImageView, vkDestroyImageView>({
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.components = components,
		.subresourceRange = subresource_range
	}, device);
}

VulkanDeviceMemory create_device_memory(VkDevice device, VkPhysicalDevice physical_device, VkMemoryRequirements requirements, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties physical_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_properties);
	std::optional<uint32_t> type_index;
	for (uint32_t i = 0; i < physical_properties.memoryTypeCount; i++) {
		if (!(requirements.memoryTypeBits & (1 << i))) {
			continue;
		}
		if ((physical_properties.memoryTypes[i].propertyFlags & properties) != properties) {
			continue;
		}
		type_index = i;
	}
	if (!type_index) {
		ERROR("Failed to find suitable memory type!");
	}
	return create_wrapper<vkAllocateMemory, vkFreeMemory>({
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = requirements.size,
		.memoryTypeIndex = type_index.value()
	}, device);
}

VulkanSurface create_surface(VkInstance instance, GLFWwindow* window)
{
	VkSurfaceKHR* output = new VkSurfaceKHR();
	CHECK_VULKAN(glfwCreateWindowSurface(instance, window, nullptr, output));
	auto destroy = [=](VkSurfaceKHR* ptr) {
		vkDestroySurfaceKHR(instance, *ptr, nullptr);
	};
	return Wrapper<VkSurfaceKHR>(output, destroy);
}

VulkanSwapchain create_swapchain(VkDevice device, VkSwapchainCreateInfoKHR info)
{
	return create_wrapper<vkCreateSwapchainKHR, vkDestroySwapchainKHR>(
		info, device
	);
}

VulkanPipeline create_pipeline(VkDevice device, VkComputePipelineCreateInfo info)
{
	VkPipeline* output = new VkPipeline();
	CHECK_VULKAN(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, output));
	auto destroy = [=](VkPipeline* ptr) {
		vkDestroyPipeline(device, *ptr, nullptr);
	};
	return Wrapper<VkPipeline>(output, destroy);
}

VulkanPipeline create_pipeline(VkDevice device, VkGraphicsPipelineCreateInfo info)
{
	VkPipeline* output = new VkPipeline();
	CHECK_VULKAN(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, output));
	auto destroy = [=](VkPipeline* ptr) {
		vkDestroyPipeline(device, *ptr, nullptr);
	};
	return Wrapper<VkPipeline>(output, destroy);
}

VulkanBuffer create_buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage)
{
	return create_wrapper<vkCreateBuffer, vkDestroyBuffer>({
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	}, device);
}

}
