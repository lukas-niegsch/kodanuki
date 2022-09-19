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
}

//////////////////////////////// Specializations ///////////////////////////////
template <>
void print_vulkan_struct(VkExtensionProperties info)
{
	std::cout << "extensionName: " << info.extensionName << '\n';
	std::cout << "specVersion: ";
	std::cout << VK_VERSION_MAJOR(info.specVersion) << ".";
	std::cout << VK_VERSION_MINOR(info.specVersion) << ".";
	std::cout << VK_VERSION_PATCH(info.specVersion) << "\n";
}
