#pragma once
#include <vulkan/vulkan.h>
#include <functional>
#include <memory>
#include <vector>

namespace kodanuki
{

/**
 * Contains all the configurable information for creating a device.
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
struct DeviceBuilder
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
class VulkanDevice
{
public:
	// Creates a new vulkan device from the given builder.
	VulkanDevice(DeviceBuilder builder);

	// Returns the handle to the logical device.
	operator VkDevice() const;

public:
	// Returns the handle to the instance.
	VkInstance instance();

	// Returns the handle to the physical device.
	VkPhysicalDevice physical_device();

	// Returns the handles to the queues.
	std::vector<VkQueue> queues();

	// Returns the used queue family index.
	uint32_t queue_family_index();

	// Returns the globally used descriptor pool.
	VkDescriptorPool get_descriptor_pool();

	// Returns the globally used command pool.
	VkCommandPool get_command_pool();

public:
	// Executes the given command for the device.
	float execute(std::function<void(VkCommandBuffer)> command, bool debug = false);

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
	VkDescriptorPool create_descriptor_pool();

	/**
	 * Creates the global command pool.
	 *
	 * This command pool should only be used inside the main thread. Any
	 * external thread must create its own command pool. Any command buffer
	 * allocated using this pool must be freed explicitely.
	 *
	 * @return The global command pool.
	 */
	VkCommandPool create_command_pool();

private:
	// The abstract pointer to the implementation.
	std::shared_ptr<struct DeviceState> pimpl;
};

}
