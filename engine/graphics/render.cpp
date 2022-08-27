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
}

void RenderModule::onDetach()
{
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

}
