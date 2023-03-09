#pragma once
#include "engine/vulkan/device.h"
#include "engine/utility/wrapper.h"
#include <vulkan/vulkan.h>

namespace kodanuki
{

using VulkanShaderModule = Wrapper<VkShaderModule>;
using VulkanFence = Wrapper<VkFence>;
using VulkanSemaphore = Wrapper<VkSemaphore>;
using VulkanDescriptorPool = Wrapper<VkDescriptorPool>;

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
 * Descriptor pools maintain multiple descriptors.
 *
 * @param device The device that stores the descriptor pool.
 * @param pool_sizes The sizes of the descriptor pool for each type.
 */
VulkanDescriptorPool create_descriptor_pool(VkDevice device, const std::vector<VkDescriptorPoolSize> pool_sizes);

}
