#pragma once
#include <vulkan/vulkan.h>
#include <vector>

std::vector<VkExtensionProperties> getInstanceExtensionProperties(const char* layerName);

std::vector<VkLayerProperties> getInstanceLayerProperties();

std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance);

std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice device);

std::vector<VkExtensionProperties> getDeviceExtensionProperties(VkPhysicalDevice device, const char* layer);

std::vector<VkSurfaceFormatKHR> getPhysicalDeviceSurfaceFormats(VkPhysicalDevice device, VkSurfaceKHR surface);

std::vector<VkPresentModeKHR> getPhysicalDeviceSurfacePresentModes(VkPhysicalDevice device, VkSurfaceKHR surface);

std::vector<VkImage> getSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain);
