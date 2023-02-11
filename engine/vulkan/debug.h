#pragma once
#include "engine/utility/signature.h"
#include "engine/utility/type_name.h"
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>
#define LINE_LENGTH 80

#define ERROR(reason)							\
	do {										\
		std::stringstream err;					\
		err << reason << '\n';					\
		err << "File: " << __FILE__ << '\n';	\
		err << "Line: " << __LINE__ << '\n';	\
		std::cout << err.str();					\
		std::terminate(); 						\
	} while (false)

#define CHECK_VULKAN(result)					\
	do {										\
		auto return_type = result;				\
		if (return_type != VK_SUCCESS) {		\
			ERROR(vulkan_debug(return_type));	\
		}										\
	} while (false)

namespace kodanuki
{

/**
 * Prints the values of the given structure.
 * 
 * This function will always throw a compiler error
 * unless it is specialized for the given type.
 * 
 * @param info The structure that should be converted to a string.
 */
template <typename T>
std::string vulkan_debug(T info) = delete; // No debug info supported!

/**
 * Prints the values of the given structure.
 * 
 * This function will always throw a compiler error
 * unless it is specialized for the given type.
 * 
 * @param infos The structures that should be converted to a string.
 */
template <typename T>
std::string vulkan_debug(std::vector<T> infos)
{
	std::stringstream ss;
	for (T info : infos) {
		ss << vulkan_debug<T>(info) << '\n';
	}
	return ss.str();
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
void print_vulkan_struct(T info)
{
	std::cout << vulkan_debug<T>(info);
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
	std::cout << "[Info] " << kodanuki::type_name<T>() << '\n';
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
	std::cout << "[Info] Vector of " << kodanuki::type_name<T>() << '\n';
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
	using Q = kodanuki::reverse_signature_t<0, Function>;
	using T = std::remove_pointer_t<Q>;
	uint32_t size;
	Function(args... , &size, nullptr);
	std::vector<T> result(size);
	Function(args... , &size, result.data());
	return result;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Specializations ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <>
std::string vulkan_debug(VkResult info);

template <>
std::string vulkan_debug(VkExtensionProperties info);

template <>
std::string vulkan_debug(VkLayerProperties info);

template <>
std::string vulkan_debug(VkPhysicalDeviceProperties info);

template <>
std::string vulkan_debug(VkQueueFamilyProperties info);

template <>
std::string vulkan_debug(VkPhysicalDeviceFeatures info);

template <>
std::string vulkan_debug(VkSurfaceFormatKHR info);

template <>
std::string vulkan_debug(VkPresentModeKHR info);

template <>
std::string vulkan_debug(VkSurfaceCapabilitiesKHR info);

template <>
std::string vulkan_debug(VkPhysicalDeviceMemoryProperties info);

template <>
std::string vulkan_debug(VkMemoryHeap info);

template <>
std::string vulkan_debug(VkMemoryType info);

template <>
std::string vulkan_debug(VkFormat info);

template <>
std::string vulkan_debug(VkVertexInputAttributeDescription info);

}
