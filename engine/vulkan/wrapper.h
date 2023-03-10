#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include <functional>

namespace kodanuki
{

/**
 * Shared wrapper around some given vulkan type.
 *
 * Each wrapper basically behaves the same way as a shared_ptr but it
 * also convertes to the base type. This just makes it easier to use
 * them in methods without having to dereference it. Also allows the
 * user to specify some custom destroy method.
 */
template <typename T>
struct Wrapper
{
public:
	/**
	 * Default constructor so this class plays nicely with std::shared_ptr.
	 */
	Wrapper() = default;

	/**
	 * Creates a new wrapper around the pointer.
	 *
	 * This wrapper now owns the pointer. It will destroy the pointer once
	 * this class is no longer used.
	 *
	 * @param ptr The pointer which will be wrapped.
	 * @param destroy The custom destroy method.
	 */
	Wrapper(T* ptr, std::function<void(T*)> destroy)
	{
		this->state = std::shared_ptr<T>(ptr, destroy);
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator T&()
	{
		return *state;
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator const T&() const
	{
		return *state;
	}

private:
	std::shared_ptr<T> state;
};

using VulkanShaderModule = Wrapper<VkShaderModule>;
using VulkanFence = Wrapper<VkFence>;
using VulkanSemaphore = Wrapper<VkSemaphore>;
using VulkanDescriptorPool = Wrapper<VkDescriptorPool>;
using VulkanDescriptorSet = Wrapper<VkDescriptorSet>;
using VulkanCommandPool = Wrapper<VkCommandPool>;
using VulkanCommandBuffer = Wrapper<VkCommandBuffer>;
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
 * @return The wrapper around the vulkan descriptor pool.
 */
VulkanDescriptorPool create_descriptor_pool(VkDevice device, const std::vector<VkDescriptorPoolSize> pool_sizes);

/**
 * Descriptor sets hold the information for pipelines.
 *
 * @param device The device that stores the descriptor set.
 * @param pool The pool which maintains the descriptor set.
 * @param layout The layout with which the descriptor set is created.
 * @return The wrapper around the vulkan descriptor set.
 */
VulkanDescriptorSet create_descriptor_set(VkDevice device, VkDescriptorPool pool, const VkDescriptorSetLayout layout);

/**
 * Command pools maintain multiple command buffers.
 *
 * @param device The device that stores the command pool.
 * @param queue_family_index The queue family on which the command buffers operate.
 * @return The wrapper around the vulkan command pool.
 */
VulkanCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index);

/**
 * Command buffers hold the information for queue commands.
 *
 * @param device The device that stores the command buffer.
 * @param pool The pool which maintains the command buffer.
 * @return The wrapper around the vulkan command buffer.
 */
VulkanCommandBuffer create_command_buffer(VkDevice device, VkCommandPool pool);

/**
 * Query pools maintain multiple timestamps.
 *
 * @param device The device that stores the query pool.
 * @param time_stamps The maximum number of timestamps.
 * @return The wrapper around the vulkan query pool.
 */
VulkanQueryPool create_query_pool(VkDevice device, uint32_t time_stamps);

}
