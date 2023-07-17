#pragma once
#include "engine/central/utility/debug.h"
#include "extern/SPIRV-Reflect/spirv_reflect.h"
#include <vulkan/vulkan.h>

#define CHECK_VULKAN(result) 				\
	CHECK_RESULT(result, VK_SUCCESS)

#define CHECK_SPIRV(result)					\
	CHECK_RESULT(result, SPV_REFLECT_RESULT_SUCCESS)

namespace kodanuki
{

template <>
std::string stringify(VkBool32 info);

template <>
std::string stringify(VkResult info);

template <>
std::string stringify(VkFormat info);

template <>
std::string stringify(VkFlags info);

template <>
std::string stringify(VkLayerProperties info);

template <>
std::string stringify(VkExtensionProperties info);

template <>
std::string stringify(VkPhysicalDeviceType info);

template <>
std::string stringify(VkPhysicalDeviceFeatures info);

template <>
std::string stringify(VkPhysicalDeviceProperties info);

template <>
std::string stringify(VkPhysicalDeviceMemoryProperties info);

template <>
std::string stringify(VkPhysicalDeviceSubgroupProperties info);

template <>
std::string stringify(VkQueueFamilyProperties info);

template <>
std::string stringify(VkSurfaceFormatKHR info);

template <>
std::string stringify(VkPresentModeKHR info);

template <>
std::string stringify(VkSurfaceCapabilitiesKHR info);

template <>
std::string stringify(VkVertexInputAttributeDescription info);

template <>
std::string stringify(VkMemoryHeap info);

template <>
std::string stringify(VkMemoryType info);

template <>
std::string stringify(SpvReflectResult info);

}
