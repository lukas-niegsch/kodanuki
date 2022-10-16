#pragma once
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <functional>

namespace Kodanuki
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
struct DeviceCreateInfo
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
 *
 * Instances can be copied around freely and will release all
 * ressources once unused.
 */
class VulkanDevice
{
public:
	/**
	 * Creates a new vulkan device from the given builder.
	 *
	 * @param builder The information on how to build the device.
	 */
	VulkanDevice(DeviceCreateInfo builder);

public:
	// Returns the handle to the instance.
	VkInstance get_instance();

	// Returns the handle to the physical device.
	VkPhysicalDevice get_physical_device();

	// Returns the handle to the logical device.
	VkDevice get_logical_device();

private:
	// Destroys unused devices automatically.
	std::shared_ptr<Entity> pimpl;
};

}
