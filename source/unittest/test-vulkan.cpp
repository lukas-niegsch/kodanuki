#include "engine/vulkan/device.h"
#include "engine/vulkan/tensor.h"
#include <doctest/doctest.h>
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

VulkanDevice create_default_device()
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

TEST_CASE("Tensor API usage example")
{
	// Each vulkan tensor requires a vulkan device for executing commands.
	VulkanDevice device = create_default_device();
	
	// Heres how to create some tensors allocating memory on the device.
	VulkanTensor a = {{
		.device = device,
		.shape = {3, 4, 5},
		.dtype = VulkanTensor::eFloat,
		.dshare = VulkanTensor::eUnique
	}};

	// Some of the values can be omitted when using the default values.
	VulkanTensor b = {{
		.device = device,
		.shape = {3, 4, 5},
		.dshare = VulkanTensor::eShared
	}};

	// The tensors have allocated their memory, but are currently empty.
	// We can load data and do other things with the tensor using operators.
	// An operator can be anything that has input tensors and produces some
	// output tensors. Inplace-Operators have an "i" before the name.
	vt::ifill(a, 0.0f);
	vt::ifill(b, 1.0f);
	VulkanTensor c = vt::add(a, b);
	vt::icos(c);

	// Inplace-Operators don't produce additional memory, but modify the
	// tensor values directly. Tensors will deallocate their memory once
	// they are no longer used.
}

TEST_CASE("access and modification of tensor memory is possible")
{
	VulkanDevice device = create_default_device();
	VulkanTensor a = {{
		.device = device,
		.shape = {3, 4, 5}
	}};

	a.with_maps<float>([](std::vector<float>& values) {
		for (std::size_t i = 0; i < values.size(); i++) {
			values[i] = i;
		}
	});

	a.with_maps<float>([](std::vector<float>& values) {
		for (std::size_t i = 0; i < values.size(); i++) {
			CHECK(values[i] == doctest::Approx(i));
		}
	});
}