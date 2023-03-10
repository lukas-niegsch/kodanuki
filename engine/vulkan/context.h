#pragma once
#include "engine/vulkan/wrapper.h"
#include <vulkan/vulkan.h>
#include <functional>
#include <memory>
#include <vector>

namespace kodanuki
{

/**
 * Contains all the configurable information for creating a context.
 *
 * These values will be considered as much as possible when selecting
 * the graphics card and how the logical device is created. The caller
 * has to make sure that the provided values make sense.
 *
 * Example:
 * The layers/extensions are available.
 * The user has a gpu with graphics compute capabilities.
 * The gpu has at least the selected amount of queue families.
 */
struct ContextBuilder
{
	// Enables the given layers for the vulkan instance.
	std::vector<const char*> instance_layers;

	// Enables the given extensions for the vulkan instance.
	std::vector<const char*> instance_extensions;

	// Enables the given extensions for the vulkan device.
	std::vector<const char*> device_extensions;

	// Selects the device with the best score.
	std::function<int(VkPhysicalDevice)> gpu_score;

	// Selects the queue with the best score.
	std::function<int(VkQueueFamilyProperties)> queue_score;

	// The number of queues and their priorities.
	std::vector<float> queue_priorities;
};

/**
 * The vulkan device is a wrapper around the graphics card.
 *
 * Each vulkan device creates its own vulkan instance and picks
 * some graphics card based on the provided builder. This class
 * handles the interaction with the graphics card.
 */
class VulkanContext
{
public:
	// Creates a new vulkan device from the given builder.
	VulkanContext(ContextBuilder builder);

	// Returns the handle to the logical device.
	operator VkDevice() const;

	// Returns the handle to the instance.
	VkInstance get_instance();

	// Returns the handle to the physical device.
	VkPhysicalDevice get_physical_device();

	// Returns the handles to the queues.
	std::vector<VkQueue> get_queues();

	// Returns the used queue family index.
	uint32_t get_queue_family();

	// Returns the globally used descriptor pool.
	VkDescriptorPool get_descriptor_pool();

	// Returns the globally used command pool.
	VkCommandPool get_command_pool();

public:
	/**
	 * Executes the given command using the last queue.
	 *
	 * We wait for the command to finish and measure the time device took
	 * to execute the command. This time will not include the waiting for
	 * example if other commands are executed inside the same queue.
	 * 
	 * @param command The command that should be executed.
	 * @return The time the command took in nanoseconds.
	 */
	float execute(std::function<void(VkCommandBuffer)> command);

	/**
	* Execute the given callback closure with the command buffer.
	*
	* This will call the following functions:
	* vkResetCommandBuffer
	* vkBeginCommandBuffer
	* vkEndCommandBuffer
	*
	* @param buffer The command buffer used inside the closure.
	* @param closure The function that will be executed.
	*/
	void with_command_buffer(VkCommandBuffer buffer, std::function<void(VkCommandBuffer)> closure);

private:
	/**
	 * Creates the global descriptor pool.
	 *
	 * This descriptor pool is very large and be used throughout the
	 * application. Any descriptor allocated using this pool must be
	 * freed explicitely.
	 *
	 * @return The global descriptor pool.
	 */
	VulkanDescriptorPool create_default_descriptor_pool();

private:
	VulkanInstance instance;
	VulkanDevice logical_device;
	VulkanCommandPool command_pool;
	VulkanCommandBuffer execute_buffer;
	VulkanQueryPool query_pool;
	VulkanDescriptorPool descriptor_pool;
	VkPhysicalDevice physical_device;
	std::vector<VkQueue> queues;
	VkQueue execute_queue;
	uint32_t queue_family;
};

}
