#pragma once
#include "engine/utility/template/signature.h"
#include "engine/utility/template/type_name.h"
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#define LINE_LENGTH 80

#define CHECK_VULKAN(result) \
	if (result != VK_SUCCESS) { \
		std::cout << "[Vulkan Error] " << result << '\n'; \
		throw std::runtime_error(""); \
	}

namespace Kodanuki
{

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
	using Q = Kodanuki::reverse_signature_t<0, Function>;
	using T = std::remove_pointer_t<Q>;
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
 * Prints the values of the given structure.
 * 
 * This function will always throw a compiler error
 * unless it is specialized for the given type.
 * 
 * @param info The structure that should be printed.
 */
template <typename T>
void print_vulkan_struct(std::vector<T> info)
{
	for (int i = 0; i < (int) info.size(); i++) {
		if (i != 0) {
			std::cout << std::string(LINE_LENGTH, ' ') << '\n';
		}
		print_vulkan_struct<T>(info[i]);
	}
}

/**
 * Prints the values from the given structure and
 * surrounds it with some formatting.
 * 
 * @param info The structure that should be printed.
 */
template <typename T>
void print_vulkan_info(T info)
{
	std::cout << "[Info] " << Kodanuki::type_name<T>() << '\n';
	std::cout << std::string(LINE_LENGTH, '=') << '\n';
	print_vulkan_struct<T>(info);
	std::cout << std::string(LINE_LENGTH, '=') << '\n';
	std::cout << std::endl;
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
	std::cout << "[Info] Vector of " << Kodanuki::type_name<T>() << '\n';
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

//////////////////////////////// Specializations ///////////////////////////////
template <>
void print_vulkan_struct(VkExtensionProperties info);

template <>
void print_vulkan_struct(VkLayerProperties info);

template <>
void print_vulkan_struct(VkPhysicalDeviceProperties info);

template <>
void print_vulkan_struct(VkQueueFamilyProperties info);

template <>
void print_vulkan_struct(VkPhysicalDeviceFeatures info);

template <>
void print_vulkan_struct(VkSurfaceFormatKHR info);

template <>
void print_vulkan_struct(VkPresentModeKHR info);

template <>
void print_vulkan_struct(VkSurfaceCapabilitiesKHR info);

template <>
void print_vulkan_info(VkPhysicalDevice info);

}
