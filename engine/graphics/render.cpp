#include "engine/graphics/render.h"
#include "engine/graphics/vulkan/debug.h"
#include "engine/graphics/vulkan/enumerate.h"
#include <algorithm>

namespace Kodanuki
{

RenderModule::RenderModule(GameInfo& info, std::shared_ptr<WindowModule> window)
	: info(info), window(window) {}

void RenderModule::onAttach()
{
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

void RenderModule::onDetach()
{
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void RenderModule::createInstance()
{
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = info.title;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.pEngineName = "Kodanuki";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	uint32_t count;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);
	std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = layers.size();
	instanceInfo.ppEnabledLayerNames = layers.data();
	instanceInfo.enabledExtensionCount = extensions.size();
	instanceInfo.ppEnabledExtensionNames = extensions.data();

	auto result = vkCreateInstance(&instanceInfo, nullptr, &instance);
	VERIFY_VULKAN_RESULT(result);
}

void RenderModule::createSurface()
{
	surface = window->createSurface(instance);
}

bool RenderModule::isDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

void RenderModule::pickPhysicalDevice()
{
	auto devices = getPhysicalDevices(instance);
	auto it = std::find_if(devices.begin(), devices.end(), [&](auto device) {
		return isDeviceSuitable(device);
	});
	physicalDevice = *it;
	// printPhysicalDeviceProperties(devices);
	// printPhysicalDeviceProperties(physicalDevice);
}

bool RenderModule::isQueueFamilySuitable(VkQueueFamilyProperties family)
{
	return family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
}

void RenderModule::createLogicalDevice()
{
	auto queueFamilies = getQueueFamilyProperties(physicalDevice);
	// printQueueFamilyProperties(queueFamilies);

	auto it = std::find_if(queueFamilies.begin(), queueFamilies.end(), [&](auto family) {
		return isQueueFamilySuitable(family);
	});
	queueCount = it->queueCount;
	queueFamilyIndex = std::distance(queueFamilies.begin(), it);
	std::vector<float> priorities(queueCount, 0.8);

	VkDeviceQueueCreateInfo queueCreateInfo;
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = nullptr;
	queueCreateInfo.flags = 0;
	queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	queueCreateInfo.queueCount = queueCount;
	queueCreateInfo.pQueuePriorities = priorities.data();

	// VkPhysicalDeviceFeatures contains lots of bools, set them to 0 for now
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// printExtensionProperties(getDeviceExtensionProperties(physicalDevice, nullptr));
	std::vector<const char *> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	auto result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
	VERIFY_VULKAN_RESULT(result);
}

void RenderModule::createSwapChain()
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
	// printSurfaceCapabilitiesKHR(surfaceCapabilities);

	// printSurfaceFormatKHR(getPhysicalDeviceSurfaceFormats(physicalDevice, surface));
	// printPresentModeKHR(getPhysicalDeviceSurfacePresentModes(physicalDevice, surface));

	imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	imageExtend = surfaceCapabilities.currentExtent;

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = 3;
	swapchainCreateInfo.imageFormat = imageFormat;
	swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCreateInfo.imageExtent = imageExtend;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 1;
	swapchainCreateInfo.pQueueFamilyIndices = &queueFamilyIndex;
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapchainCreateInfo.clipped = true;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	auto result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
	VERIFY_VULKAN_RESULT(result);

	images = getSwapchainImagesKHR(device, swapchain);
}

}
