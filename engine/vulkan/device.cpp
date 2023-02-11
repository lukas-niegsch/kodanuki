#include "engine/vulkan/device.h"
#include "engine/vulkan/debug.h"
#include <cassert>

namespace kodanuki
{

VkInstance create_instance(std::vector<const char*> layers, std::vector<const char*> extensions)
{
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;
	app_info.pApplicationName = nullptr;
	app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	app_info.pEngineName = "kodanuki";
	app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	app_info.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instance_info;
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pNext = nullptr;
	instance_info.flags = 0;
	instance_info.pApplicationInfo = &app_info;
	instance_info.enabledLayerCount = layers.size();
	instance_info.ppEnabledLayerNames = layers.data();
	instance_info.enabledExtensionCount = extensions.size();
	instance_info.ppEnabledExtensionNames = extensions.data();

	VkInstance result;
	CHECK_VULKAN(vkCreateInstance(&instance_info, nullptr, &result));
	return result;
}

VkPhysicalDevice select_physical_device(VkInstance instance, std::function<int(VkPhysicalDevice)> gpu_score)
{
	auto devices = vectorize<vkEnumeratePhysicalDevices>(instance);
	auto compare = [&](auto a, auto b) { return gpu_score(a) < gpu_score(b); };
	assert(devices.size() > 0); // No graphics card available!
	return *std::max_element(devices.begin(), devices.end(), compare);
}

uint32_t select_queue_family_index(VkPhysicalDevice device, std::function<int(VkQueueFamilyProperties)> queue_score)
{
	auto queues = vectorize<vkGetPhysicalDeviceQueueFamilyProperties>(device);
	auto compare = [&](auto a, auto b) { return queue_score(a) < queue_score(b); };
	return std::distance(queues.begin(), std::max_element(queues.begin(), queues.end(), compare));
}

VkDeviceQueueCreateInfo select_queue_family(uint32_t queue_index, std::vector<float>& properties)
{
	VkDeviceQueueCreateInfo queue_info;
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext = nullptr;
	queue_info.flags = 0;
	queue_info.queueFamilyIndex = queue_index;
	queue_info.queueCount = properties.size();
	queue_info.pQueuePriorities = properties.data();
	return queue_info;
}

VkDevice create_logical_device(VkPhysicalDevice physical_device, VkDeviceQueueCreateInfo queue_family, std::vector<const char*> extensions)
{
	VkPhysicalDeviceFeatures device_features = {};

	VkDeviceCreateInfo device_info;
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = nullptr;
	device_info.flags = 0;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_family;
	device_info.enabledLayerCount = 0;
	device_info.ppEnabledLayerNames = nullptr;
	device_info.enabledExtensionCount = extensions.size();
	device_info.ppEnabledExtensionNames = extensions.data();
	device_info.pEnabledFeatures = &device_features;

	VkDevice result;
	CHECK_VULKAN(vkCreateDevice(physical_device, &device_info, nullptr, &result));
	return result;
}

std::vector<VkQueue> get_queue_handles(VkDevice logical_device, VkDeviceQueueCreateInfo queue_family)
{
	std::vector<VkQueue> result(queue_family.queueCount);
	
	for (int i = 0; i < (int) result.size(); i++) {
		vkGetDeviceQueue(logical_device, queue_family.queueFamilyIndex, 0, &result[i]);
	}
	
	return result;
}

struct DeviceState
{
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice logical_device;
	uint32_t queue_index;
	std::vector<VkQueue> queues;
	~DeviceState();
};

DeviceState::~DeviceState()
{
	CHECK_VULKAN(vkDeviceWaitIdle(logical_device));
	vkDestroyDevice(logical_device, nullptr);
	vkDestroyInstance(instance, nullptr);
}

VulkanDevice::VulkanDevice(DeviceBuilder builder)
{
	auto instance = create_instance(builder.instance_layers, builder.instance_extensions);
	auto physical_device = select_physical_device(instance, builder.gpu_score);
	auto queue_index = select_queue_family_index(physical_device, builder.queue_score);
	auto queue_family = select_queue_family(queue_index, builder.queue_priorities);
	auto logical_device = create_logical_device(physical_device, queue_family, builder.device_extensions);
	auto queues = get_queue_handles(logical_device, queue_family);
	pimpl = std::make_shared<DeviceState>(instance, physical_device, logical_device, queue_index, queues);
}

VulkanDevice::operator VkDevice()
{
	return pimpl->logical_device;
}

VkInstance VulkanDevice::instance()
{
	return pimpl->instance;
}

VkPhysicalDevice VulkanDevice::physical_device()
{
	return pimpl->physical_device;
}

std::vector<VkQueue> VulkanDevice::queues()
{
	return pimpl->queues;
}

uint32_t VulkanDevice::queue_family_index()
{
	return pimpl->queue_index;
}

}
