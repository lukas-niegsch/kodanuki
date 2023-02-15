#include "source/unittest/tensors/default_device.h"
using namespace kodanuki;

int score_physical_device(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

int score_queue_family(VkQueueFamilyProperties family)
{
	int score = family.queueCount;
	score *= family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	return score;
}

VulkanDevice default_device()
{
	VulkanDevice device = {{
		.instance_layers = {"VK_LAYER_KHRONOS_validation"},
		.instance_extensions = {},
		.device_extensions = {},
		.gpu_score = &score_physical_device,
		.queue_score = &score_queue_family,
		.queue_priorities = {1.0f}
	}};

	return device;
}
