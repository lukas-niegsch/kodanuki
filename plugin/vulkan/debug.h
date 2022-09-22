#pragma once
#include "engine/utility/types.h"
#include <vulkan/vulkan.h>
#include <iostream>
#include <tuple>
#include <vector>
#define LINE_LENGTH 80

/**
 * Vectorizes properties that can be enumerated using vulkan.
 * 
 * This essentially only is a wrapper around vulkan's enumerate
 * pattern. Instead of having to calling these functions twice
 * (first to get the count, then to get the values), we return
 * a vector containing all the values instead.
 * 
 * usage:
 * vectorize<vkEnumerateInstanceExtensionProperties>(nullptr);
 */
template <auto Function, typename ... Args>
auto vectorize(Args ... args)
{
	using Q = FinalArgument<decltype(Function)>::type;
	using T = std::remove_pointer<Q>::type;
	uint32_t size;
	Function(args... , &size, nullptr);
	std::vector<T> result(size);
	Function(args... , &size, result.data());
	return result;
}

/**
 * Prints the values of the given structure.
 * 
 * This function will always throw a compiler error
 * unless it is specialized for the given type.
 * 
 * @param info The structure that should be printed.
 */
template <typename T>
void print_vulkan_struct(T info) = delete; // No debug info supported!

/**
 * Prints the values from the given structure and
 * surrounds it with some formatting.
 * 
 * @param info The structure that should be printed.
 */
template <typename T>
void print_vulkan_info(T info)
{
	std::cout << "[Info] " << type_name<T>() << '\n';
	std::cout << std::string(LINE_LENGTH, '=') << '\n';
	print_vulkan_struct<T>();
	std::cout << std::string(LINE_LENGTH, '=') << '\n';
}

/**
 * Prints the values from the given structure and
 * surrounds it with some formatting.
 * 
 * @param info The structure that should be printed.
 */
template <typename T>
void print_vulkan_info(std::vector<T> info)
{
	std::cout << "[Info] Vector of " << type_name<T>() << '\n';
	std::cout << std::string(LINE_LENGTH, '=') << '\n';
	for (int i = 0; i < (int) info.size(); i++) {
		if (i != 0) {
			std::cout << std::string(LINE_LENGTH, '-') << '\n';
		}
		print_vulkan_struct<T>(info[i]);
	}
	std::cout << std::string(LINE_LENGTH, '=') << '\n';
	std::cout << std::endl;
}

void print_vulkan_version(std::string name, uint32_t version)
{
	std::cout << name << ": ";
	std::cout << VK_VERSION_MAJOR(version) << ".";
	std::cout << VK_VERSION_MINOR(version) << ".";
	std::cout << VK_VERSION_PATCH(version) << "\n";
}

//////////////////////////////// Specializations ///////////////////////////////
template <>
void print_vulkan_struct(VkExtensionProperties info)
{
	std::cout << "extensionName: " << info.extensionName << '\n';
	print_vulkan_version("specVersion", info.specVersion);
}

template <>
void print_vulkan_struct(VkLayerProperties info)
{
	std::cout << "layerName: " << info.layerName << '\n';
	print_vulkan_version("specVersion", info.specVersion);
	print_vulkan_version("implementationVersion", info.implementationVersion);
	std::cout << "description: " << info.description << '\n';
}

template <>
void print_vulkan_struct(VkPhysicalDeviceProperties info)
{
	print_vulkan_version("apiVersion", info.apiVersion);
	print_vulkan_version("driverVersion", info.driverVersion);
	std::cout << "vendorID: " << info.vendorID << '\n';
	std::cout << "deviceID: " << info.deviceID << '\n';
	std::cout << "deviceType: ";
	switch(info.deviceType)
	{
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_OTHER";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_CPU";
		break;
	default:
		std::cout << "unknown";
	}
	std::cout << '\n';
	std::cout << "deviceName: " << info.deviceName << '\n';
	std::cout << "pipelineCacheUUID: " << info.pipelineCacheUUID << '\n';
}

template <>
void print_vulkan_struct(VkPhysicalDevice info)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(info, &properties);
	print_vulkan_struct(properties);
}

