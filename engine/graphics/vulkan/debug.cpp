#include "debug.h"
#include <iostream>
#include <algorithm>

void printExtensionProperties(VkExtensionProperties extension)
{
	std::cout << "extensionName: " << extension.extensionName << '\n';
	std::cout << '\n';
}

void printExtensionProperties(std::vector<VkExtensionProperties> extensions)
{
	std::cout << "List of VkExtensionProperties:" << '\n';
	std::cout << "================================" << '\n';
	for (auto extension : extensions)
		printExtensionProperties(extension);
	std::cout << "================================" << '\n';
	std::cout << std::endl;
}

void printLayerProperties(VkLayerProperties layer)
{
	std::cout << "layerName: " << layer.layerName << '\n';
	std::cout << "description: " << layer.description << '\n';
	std::cout << '\n';
}

void printLayerProperties(std::vector<VkLayerProperties> layers)
{
	std::cout << "List of VkLayerProperties:" << '\n';
	std::cout << "================================" << '\n';
	for (auto layer : layers)
		printLayerProperties(layer);
	std::cout << "================================" << '\n';
	std::cout << std::endl;
}

void printPhysicalDeviceProperties(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	std::cout << "deviceName: " << deviceProperties.deviceName << '\n';
	std::cout << '\n';
}

void printPhysicalDeviceProperties(std::vector<VkPhysicalDevice> devices)
{
	std::cout << "List of VkPhysicalDeviceProperties:" << '\n';
	std::cout << "================================" << '\n';
	for (auto device : devices)
		printPhysicalDeviceProperties(device);
	std::cout << "================================" << '\n';
	std::cout << std::endl;
}

void printQueueFamilyProperties(VkQueueFamilyProperties family)
{
	std::cout << "queueCount: " << family.queueCount << '\n';
	std::cout << "VK_QUEUE_GRAPHICS_BIT: " << ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << '\n';
	std::cout << "VK_QUEUE_COMPUTE_BIT: " << ((family.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << '\n';
	std::cout << "VK_QUEUE_TRANSFER_BIT: " << ((family.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << '\n';
	std::cout << "VK_QUEUE_SPARSE_BINDING_BIT: " << ((family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << '\n';
	std::cout << "VK_QUEUE_PROTECTED_BIT: " << ((family.queueFlags & VK_QUEUE_PROTECTED_BIT) != 0) << '\n';
	std::cout << '\n';
}

void printQueueFamilyProperties(std::vector<VkQueueFamilyProperties> families)
{
	std::cout << "List of VkQueueFamilyProperties:" << '\n';
	std::cout << "================================" << '\n';
	for (auto family : families)
		printQueueFamilyProperties(family);
	std::cout << "================================" << '\n';
	std::cout << std::endl;
}

void printExtend2D(VkExtent2D extend)
{
	std::cout << extend.width << ' ' << extend.height << '\n';
}

void printSurfaceCapabilitiesKHR(VkSurfaceCapabilitiesKHR capabilities)
{
	std::cout << "VkSurfaceCapabilitiesKHR:" << '\n';
	std::cout << "================================" << '\n';
	std::cout << "minImageCount: " << capabilities.minImageCount << '\n';
	std::cout << "maxImageCount: " << capabilities.maxImageCount << '\n';
	std::cout << "currentExtent: "; printExtend2D(capabilities.currentExtent);
	std::cout << "minImageExtent: "; printExtend2D(capabilities.minImageExtent);
	std::cout << "maxImageExtent: "; printExtend2D(capabilities.maxImageExtent);
	std::cout << "maxImageArrayLayers: " << capabilities.maxImageArrayLayers << '\n';
	std::cout << "supportedTransforms: " << capabilities.supportedTransforms << '\n';
	std::cout << "currentTransform: " << capabilities.currentTransform << '\n';
	std::cout << "supportedCompositeAlpha: " << capabilities.supportedCompositeAlpha << '\n';
	std::cout << "supportedUsageFlags: " << capabilities.supportedUsageFlags << '\n';
	std::cout << "================================" << '\n';
	std::cout << std::endl;
}

void printSurfaceFormatKHR(VkSurfaceFormatKHR format)
{
	std::cout << "format: " << format.format << '\n';
	std::cout << "colorSpace: " << format.colorSpace << '\n';
	std::cout << '\n';
}

void printSurfaceFormatKHR(std::vector<VkSurfaceFormatKHR> formats)
{
	std::cout << "List of VkSurfaceFormatKHR:" << '\n';
	std::cout << "================================" << '\n';
	for (auto format : formats)
		printSurfaceFormatKHR(format);
	std::cout << "================================" << '\n';
	std::cout << std::endl;
}

void printPresentModeKHR(VkPresentModeKHR mode)
{
	std::cout << "mode: " << mode << '\n';
	std::cout << '\n';
}

void printPresentModeKHR(std::vector<VkPresentModeKHR> modes)
{
	std::cout << "List of VkPresentModeKHR:" << '\n';
	std::cout << "================================" << '\n';
	for (auto mode : modes)
		printPresentModeKHR(mode);
	std::cout << "================================" << '\n';
	std::cout << std::endl;
}
