#pragma once
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>

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

struct DeviceCreateInfo
{
    std::vector<const char*> enabled_layers;
    std::vector<const char*> enabled_extensions;
};

Device create_device(DeviceCreateInfo seed = {});
void remove_device(Device device);

}
