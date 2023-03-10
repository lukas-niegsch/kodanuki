#include "engine/vulkan/wrapper.h"
#include "engine/vulkan/debug.h"
constexpr uint32_t VK_NULL_BIT = 0;

namespace kodanuki
{

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

VulkanCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index)
{
	return create_wrapper<vkCreateCommandPool, vkDestroyCommandPool>({
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queue_family_index
	}, device);
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

}
