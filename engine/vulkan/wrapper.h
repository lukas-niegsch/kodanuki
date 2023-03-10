#pragma once
#include "engine/utility/wrapper.h"
#include <vulkan/vulkan.h>

namespace kodanuki
{

using VulkanShaderModule = Wrapper<VkShaderModule>;
using VulkanFence = Wrapper<VkFence>;
using VulkanSemaphore = Wrapper<VkSemaphore>;
using VulkanDescriptorPool = Wrapper<VkDescriptorPool>;
using VulkanCommandPool = Wrapper<VkCommandPool>;
using VulkanQueryPool = Wrapper<VkQueryPool>;

/**
 * Shader modules contain shader code that the device can execute.
 *
 * @param device The device that stores the shader.
 * @param code The bytes of the shader SPIRV shader file.
 * @return The wrapper around the vulkan shader module.
 */
VulkanShaderModule create_shader_module(VkDevice device, std::vector<char> code);

/**
 * Fences are synchronization primitives used for waiting inside the host.
 *
 * @param device The device that stores the fence.
 * @param flags The flags with which the fence is created.
 * @return The wrapper around the vulkan fence.
 */
VulkanFence create_fence(VkDevice device, VkFenceCreateFlagBits flags);

/**
 * Semaphores are synchronization primitives used for waiting inside the device.
 *
 * @param device The device that stores the semaphore.
 * @return The wrapper around the vulkan semaphore.
 */
VulkanSemaphore create_semaphore(VkDevice device);

/**
 * Descriptor pools maintain multiple descriptor sets.
 *
 * @param device The device that stores the descriptor pool.
 * @param pool_sizes The sizes of the descriptor pool for each type.
 * @param The wrapper around the vulkan descriptor pool.
 */
VulkanDescriptorPool create_descriptor_pool(VkDevice device, const std::vector<VkDescriptorPoolSize> pool_sizes);

/**
 * Command pools maintain multiple command buffers.
 *
 * @param device The device that stores the command pool.
 * @param queue_family_index The queue family on which the command buffers operate.
 * @return The wrapper around the vulkan command pool.
 */
VulkanCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index);

/**
 * Query pools maintain multiple timestamps.
 *
 * @param device The device that stores the query pool.
 * @param time_stamps The maximum number of timestamps.
 * @return The wrapper around the vulkan query pool.
 */
VulkanQueryPool create_query_pool(VkDevice device, uint32_t time_stamps);

}
