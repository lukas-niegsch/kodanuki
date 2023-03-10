#include "engine/vulkan/device.h"
#include "engine/vulkan/debug.h"
#include "engine/vulkan/utility.h"
#include "engine/vulkan/wrapper.h"
#include <cassert>

namespace kodanuki
{

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
	VulkanInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice logical_device;
	uint32_t queue_index;
	std::vector<VkQueue> queues;
	VkQueue execute_queue;

	VulkanCommandBuffer execute_buffer;
	VulkanCommandPool command_pool;
	VulkanQueryPool query_pool;
	VulkanDescriptorPool descriptor_pool;
	~DeviceState();
};

DeviceState::~DeviceState()
{
	CHECK_VULKAN(vkDeviceWaitIdle(logical_device));
	execute_buffer = {};
	command_pool = {};
	descriptor_pool = {};
	query_pool = {};
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
	pimpl->execute_queue = queues.back();
	pimpl->command_pool = create_command_pool(logical_device, queue_family_index());
	pimpl->descriptor_pool = create_default_descriptor_pool();
	pimpl->execute_buffer = create_command_buffer(logical_device, pimpl->command_pool);
	pimpl->query_pool = create_query_pool(logical_device, 2);
}

VulkanDevice::operator VkDevice() const
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

VkDescriptorPool VulkanDevice::get_descriptor_pool()
{
	return pimpl->descriptor_pool;
}

VkCommandPool VulkanDevice::get_command_pool()
{
	return pimpl->command_pool;
}

float VulkanDevice::execute(std::function<void(VkCommandBuffer)> command, bool debug)
{
	VkCommandBuffer buffer = pimpl->execute_buffer;
	CHECK_VULKAN(vkResetCommandBuffer(buffer, 0));
	VkCommandBufferBeginInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CHECK_VULKAN(vkBeginCommandBuffer(buffer, &buffer_info));

	if (debug) {
		vkCmdResetQueryPool(buffer, pimpl->query_pool, 0, 2);
		vkCmdWriteTimestamp(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, pimpl->query_pool, 0);
	}
	
	command(buffer);

	if (debug) {
		vkCmdWriteTimestamp(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, pimpl->query_pool, 1);
	}

	CHECK_VULKAN(vkEndCommandBuffer(buffer));

	VkSubmitInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.waitSemaphoreCount = 0;
	info.pWaitSemaphores = nullptr;
	info.pWaitDstStageMask = nullptr;
	info.commandBufferCount = 1;
	info.pCommandBuffers = &buffer;
	info.signalSemaphoreCount = 0;
	info.pSignalSemaphores = nullptr;

	CHECK_VULKAN(vkQueueSubmit(pimpl->execute_queue, 1, &info, nullptr));
	CHECK_VULKAN(vkQueueWaitIdle(pimpl->execute_queue));

	if (debug) {
		uint64_t ts[2];
		CHECK_VULKAN(vkGetQueryPoolResults(pimpl->logical_device, pimpl->query_pool, 0, 2, sizeof(uint64_t) * 2, ts, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT));

		return (ts[1] - ts[0]);
	}

	return 0.0f;
}

VulkanDescriptorPool VulkanDevice::create_default_descriptor_pool()
{
	std::vector<VkDescriptorPoolSize> pool_sizes = {
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 30 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 30 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 30 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 30 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 30 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 30 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 30 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 30 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 30 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 30 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 30 }
	};

	return create_descriptor_pool(pimpl->logical_device, pool_sizes);
}

}
