#include "plugin/vulkan/device.h"
#include "plugin/vulkan/debug.h"
#include <functional>
#include <cassert>

namespace Kodanuki
{

VkInstance create_instance(std::vector<const char*> layers, std::vector<const char*> extensions)
{
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;
	app_info.pApplicationName = nullptr;
	app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	app_info.pEngineName = "Kodanuki";
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

VkPhysicalDevice pick_physical_device(VkInstance instance, std::function<int(VkPhysicalDevice)> gpu_score)
{
	auto devices = vectorize<vkEnumeratePhysicalDevices>(instance);
	auto compare = [&](auto a, auto b) { return gpu_score(a) < gpu_score(b); };
	assert(devices.size() > 0); // No graphics card available!
	return *std::max_element(devices.begin(), devices.end(), compare);
}

uint32_t pick_queue_family_index(VkPhysicalDevice device, std::function<int(VkQueueFamilyProperties)> queue_score)
{
	auto queues = vectorize<vkGetPhysicalDeviceQueueFamilyProperties>(device);
	auto compare = [&](auto a, auto b) { return queue_score(a) < queue_score(b); };
	return std::distance(queues.begin(), std::max_element(queues.begin(), queues.end(), compare));
}

VkDeviceQueueCreateInfo pick_queue_family(uint32_t queue_index, std::vector<float>& properties)
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

struct Queues
{
	uint32_t queue_family_index;
	std::vector<VkQueue> queues;
};

VulkanDevice::VulkanDevice(DeviceBuilder builder)
{
	auto deleter = [this](Entity* pimpl) { shared_destructor(pimpl); };
	pimpl = std::shared_ptr<Entity>(new Entity, deleter);
	Entity device = *pimpl = ECS::create();
	
	VkInstance instance = create_instance(builder.instance_layers, builder.instance_extensions);
	VkPhysicalDevice physical_device = pick_physical_device(instance, builder.gpu_score);
	uint32_t queue_index = pick_queue_family_index(physical_device, builder.queue_score);
	VkDeviceQueueCreateInfo queue_family = pick_queue_family(queue_index, builder.queue_priorities);
	VkDevice logical_device = create_logical_device(physical_device, queue_family, builder.device_extensions);
	std::vector<VkQueue> queues = get_queue_handles(logical_device, queue_family);

	ECS::update<VkInstance>(device, instance);
	ECS::update<VkPhysicalDevice>(device, physical_device);
	ECS::update<VkDevice>(device, logical_device);
	ECS::update<Queues>(device, {queue_index, queues});
}

void VulkanDevice::shared_destructor(Entity* pimpl)
{
	CHECK_VULKAN(vkDeviceWaitIdle(*this));
	vkDestroyDevice(*this, nullptr);
	vkDestroyInstance(instance(), nullptr);
	ECS::remove<Entity>(*pimpl);
	delete pimpl;
}

VulkanDevice::operator VkDevice()
{
	return ECS::get<VkDevice>(*pimpl);
}

VkInstance VulkanDevice::instance()
{
	return ECS::get<VkInstance>(*pimpl);
}

VkPhysicalDevice VulkanDevice::physical_device()
{
	return ECS::get<VkPhysicalDevice>(*pimpl);
}

std::vector<VkQueue> VulkanDevice::queues()
{
	return ECS::get<Queues>(*pimpl).queues;
}

uint32_t VulkanDevice::queue_family_index()
{
	return ECS::get<Queues>(*pimpl).queue_family_index;
}

}
