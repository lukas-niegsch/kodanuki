#pragma once
#include "engine/vulkan/context.h"
#include <vulkan/vulkan.h>

namespace kodanuki
{

uint32_t find_memory_type(VulkanContext device, uint32_t type_filter, VkMemoryPropertyFlags properties);

}
