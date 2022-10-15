#pragma once
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <functional>

namespace Kodanuki
{

/**
 * The device hold all the information about the graphics card.
 *
 * It combines the concepts of an vulkan instance, vulkan physical
 * device, and vulkan device into one entity. Currently, it is not
 * possible to use multiple graphics card together.
 */
typedef Entity Device;

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
 * The selected gpu has at least the selected amount of queue families.
 */
struct DeviceCreateInfo
{
    // Enables the given layers for the device.
    std::vector<const char*> enabled_layers;

    // Enables the given extensions for the device.
    std::vector<const char*> enabled_extensions;

    // Selects the device with the best score.
    std::function<int(VkPhysicalDevice)> gpu_score;

    // Selects the queue with the best score.
    std::function<int(VkQueueFamilyProperties)> queue_score;

    // The number of queues and their priorities.
    std::vector<float> queue_priorities;
};

Device create_device(DeviceCreateInfo seed);
void remove_device(Device device);

}