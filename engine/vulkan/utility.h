#pragma once
#include "engine/vulkan/device.h"
#include <vulkan/vulkan.h>

namespace kodanuki
{

uint32_t find_memory_type(VulkanDevice device, uint32_t type_filter, VkMemoryPropertyFlags properties);

}
