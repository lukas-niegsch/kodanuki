#pragma once
#include "engine/vulkan/device.h"
#include <vulkan/vulkan.h>

namespace kodanuki
{

uint32_t find_memory_type(VulkanDevice device, uint32_t type_filter, VkMemoryPropertyFlags properties);

std::vector<VkDescriptorSet> create_descriptor_sets(VulkanDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout, uint32_t count);

std::vector<VkCommandBuffer> create_command_buffers(VkDevice device, VkCommandPool pool, uint32_t count);

}
