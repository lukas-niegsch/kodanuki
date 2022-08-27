#include "enumerate.h"

template <typename T, typename F, typename ... Args>
std::vector<T> vkEnumerate(F function, Args ... args)
{
	uint32_t size;
	function(args... , &size, nullptr);
	std::vector<T> result(size);
	function(args... , &size, result.data());
	return result;
}

std::vector<VkExtensionProperties> getInstanceExtensionProperties(const char* layer)
{
	return vkEnumerate<
		VkExtensionProperties,
		decltype(vkEnumerateInstanceExtensionProperties),
		const char*
	>(vkEnumerateInstanceExtensionProperties, layer);
}

std::vector<VkLayerProperties> getInstanceLayerProperties()
{
	return vkEnumerate<
		VkLayerProperties,
		decltype(vkEnumerateInstanceLayerProperties)
	>(vkEnumerateInstanceLayerProperties);
}

std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance)
{
	return vkEnumerate<
		VkPhysicalDevice,
		decltype(vkEnumeratePhysicalDevices),
		VkInstance
	>(vkEnumeratePhysicalDevices, instance);
}

std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice device)
{
	return vkEnumerate<
		VkQueueFamilyProperties,
		decltype(vkGetPhysicalDeviceQueueFamilyProperties),
		VkPhysicalDevice
	>(vkGetPhysicalDeviceQueueFamilyProperties, device);
}

std::vector<VkExtensionProperties> getDeviceExtensionProperties(VkPhysicalDevice device, const char* layer)
{
	return vkEnumerate<
		VkExtensionProperties,
		decltype(vkEnumerateDeviceExtensionProperties),
		VkPhysicalDevice,
		const char*
	>(vkEnumerateDeviceExtensionProperties, device, layer);
}

std::vector<VkSurfaceFormatKHR> getPhysicalDeviceSurfaceFormats(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	return vkEnumerate<
		VkSurfaceFormatKHR,
		decltype(vkGetPhysicalDeviceSurfaceFormatsKHR),
		VkPhysicalDevice,
		VkSurfaceKHR
	>(vkGetPhysicalDeviceSurfaceFormatsKHR, device, surface);
}

std::vector<VkPresentModeKHR> getPhysicalDeviceSurfacePresentModes(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	return vkEnumerate<
		VkPresentModeKHR,
		decltype(vkGetPhysicalDeviceSurfacePresentModesKHR),
		VkPhysicalDevice,
		VkSurfaceKHR
	>(vkGetPhysicalDeviceSurfacePresentModesKHR, device, surface);
}

std::vector<VkImage> getSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain)
{
	return vkEnumerate<
		VkImage,
		decltype(vkGetSwapchainImagesKHR),
		VkDevice,
		VkSwapchainKHR
	>(vkGetSwapchainImagesKHR, device, swapchain);
}
