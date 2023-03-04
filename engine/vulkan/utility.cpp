#include "engine/vulkan/utility.h"
#include "engine/vulkan/debug.h"

namespace kodanuki
{

uint32_t find_memory_type(VulkanDevice device, uint32_t type_filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(device.physical_device(), &memory_properties);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}

std::vector<VkDescriptorSet> create_descriptor_sets(VulkanDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout, uint32_t count)
{
	std::vector<VkDescriptorSetLayout> layouts(count, layout);
	std::vector<VkDescriptorSet> descriptor_sets(count);

	VkDescriptorSetAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.pNext = nullptr;
    allocate_info.descriptorPool = pool;
    allocate_info.descriptorSetCount = count;
    allocate_info.pSetLayouts = layouts.data();
	CHECK_VULKAN(vkAllocateDescriptorSets(device, &allocate_info, descriptor_sets.data()));

	return descriptor_sets;
}

VkDescriptorPool create_descriptor_pool(VulkanDevice device)
{
	std::vector<VkDescriptorPoolSize> pool_sizes =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * static_cast<uint32_t>(pool_sizes.size());
	pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	pool_info.pPoolSizes = pool_sizes.data();
	VkDescriptorPool descriptor_pool;
	CHECK_VULKAN(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool));
	return descriptor_pool;
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_index)
{
	VkCommandPoolCreateInfo pool_info;
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.pNext = nullptr;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = queue_index;

	VkCommandPool command_pool;
	CHECK_VULKAN(vkCreateCommandPool(device, &pool_info, nullptr, &command_pool));
	return command_pool;
}

std::vector<VkCommandBuffer> create_command_buffers(VkDevice device, VkCommandPool pool, uint32_t count)
{
	VkCommandBufferAllocateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	buffer_info.commandPool = pool;
	buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	buffer_info.commandBufferCount = count;

	std::vector<VkCommandBuffer> result(count);
	CHECK_VULKAN(vkAllocateCommandBuffers(device, &buffer_info, result.data()));
	return result;
}

}
