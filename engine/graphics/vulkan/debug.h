#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <cassert>

#define VERIFY_VULKAN_RESULT(result) \
	if (result != VK_SUCCESS) \
		std::cout << "[Error] VkResult: " << result << std::endl; \
	assert(result == VK_SUCCESS)

void printExtensionProperties(VkExtensionProperties extension);
void printExtensionProperties(std::vector<VkExtensionProperties> extensions);

void printLayerProperties(VkLayerProperties layer);
void printLayerProperties(std::vector<VkLayerProperties> layers);

void printPhysicalDeviceProperties(VkPhysicalDevice device);
void printPhysicalDeviceProperties(std::vector<VkPhysicalDevice> devices);

void printQueueFamilyProperties(VkQueueFamilyProperties family);
void printQueueFamilyProperties(std::vector<VkQueueFamilyProperties> families);

void printSurfaceCapabilitiesKHR(VkSurfaceCapabilitiesKHR capabilities);

void printSurfaceFormatKHR(VkSurfaceFormatKHR format);
void printSurfaceFormatKHR(std::vector<VkSurfaceFormatKHR> formats);

void printPresentModeKHR(VkPresentModeKHR mode);
void printPresentModeKHR(std::vector<VkPresentModeKHR> modes);
