#include "engine/vulkan/context.h"
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

VulkanContext create_default_device()
{
	VulkanContext device = {{
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
	VulkanContext device = create_default_device();
	VulkanPipelineOldCache cache;
	
	// Heres how to create some tensors allocating memory on the device.
	VulkanTensor a = {{
		.device = device,
		.cache = cache,
		.shape = {3, 4, 5},
		.dtype = VulkanTensor::eFloat,
		.dshare = VulkanTensor::eUnique
	}};

	// Some of the values can be omitted when using the default values.
	VulkanTensor b = {{
		.device = device,
		.cache = cache,
		.shape = {3, 4, 5},
		.dshare = VulkanTensor::eShared
	}};

	// The tensors have allocated their memory, but are currently empty.
	// We can load data and do other things with the tensor using operators.
	// An operator can be anything that has input tensors and produces some
	// output tensors.
	vt::fill(a, 0.0f);
	vt::fill(b, 1.0f);
	VulkanTensor c = vt::add(a, b);
	(void) c;

	// Inplace-Operators don't produce additional memory, but modify the
	// tensor values directly. Tensors will deallocate their memory once
	// they are no longer used.
}

TEST_CASE("access and modification of tensor memory is possible")
{
	VulkanContext device = create_default_device();
	VulkanPipelineOldCache cache;

	VulkanTensor a = {{
		.device = device,
		.cache = cache,
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

TEST_CASE("adding two tensors works")
{
	VulkanContext device = create_default_device();
	VulkanPipelineOldCache cache;
	VulkanTensor a = {{
		.device = device,
		.cache = cache,
		.shape = {1, 3},
		.dtype = VulkanTensor::eFloat,
		.dshare = VulkanTensor::eUnique
	}};
	a.with_maps<float>([](std::vector<float>& values) {
		values[0] = 1.2f;
		values[1] = 3.1f;
		values[2] = 0.0f;
	});
	VulkanTensor b = {{
		.device = device,
		.cache = cache,
		.shape = {1, 3},
		.dtype = VulkanTensor::eFloat,
		.dshare = VulkanTensor::eUnique
	}};
	b.with_maps<float>([](std::vector<float>& values) {
		values[0] = 1.7f;
		values[1] = 2.1f;
		values[2] = -1.0f;
	});
	VulkanTensor c = vt::add(a, b);
	c.with_maps<float>([](std::vector<float>& values) {
		CHECK(values[0] == doctest::Approx(2.9f));
		CHECK(values[1] == doctest::Approx(5.2f));
		CHECK(values[2] == doctest::Approx(-1.0f));
	});
}

TEST_CASE("tensor operator overloading works")
{
	VulkanContext device = create_default_device();
	VulkanPipelineOldCache cache;
	VulkanTensor a = {{
		.device = device,
		.cache = cache,
		.shape = {1, 3},
		.dtype = VulkanTensor::eFloat,
		.dshare = VulkanTensor::eUnique
	}};
	a.with_maps<float>([](std::vector<float>& values) {
		values[0] = 1.2f;
		values[1] = 3.1f;
		values[2] = 0.0f;
	});
	VulkanTensor b = {{
		.device = device,
		.cache = cache,
		.shape = {1, 3},
		.dtype = VulkanTensor::eFloat,
		.dshare = VulkanTensor::eUnique
	}};
	b.with_maps<float>([](std::vector<float>& values) {
		values[0] = 1.7f;
		values[1] = 2.1f;
		values[2] = -1.0f;
	});
	VulkanTensor c = {{
		.device = device,
		.cache = cache,
		.shape = {1, 3},
		.dtype = VulkanTensor::eFloat,
		.dshare = VulkanTensor::eUnique
	}};
	c.with_maps<float>([](std::vector<float>& values) {
		values[0] = 1.9f;
		values[1] = 2.1f;
		values[2] = -1.3f;
	});

	// elementwise operator overloading
	auto t1 = vt::linear(4.0f, a, 3.0f, vt::mul(b, c));
	auto t2 = vt::add(t1, 3.0f);
	VulkanTensor z = vt::pow(t2, 2);
	z.with_maps<float>([](std::vector<float>& values) {
		CHECK(values[0] == doctest::Approx(305.9f));
		CHECK(values[1] == doctest::Approx(819.667f));
		CHECK(values[2] == doctest::Approx(47.61f));
	});
}
