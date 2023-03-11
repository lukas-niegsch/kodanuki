#include "engine/vulkan/device.h"
#include "engine/vulkan/debug.h"
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

std::vector<VkQueue> get_queue_handles(VkDevice logical_device, uint32_t queue_family, uint32_t queue_count)
{
	std::vector<VkQueue> result(queue_count);
	for (uint32_t i = 0; i < result.size(); i++) {
		vkGetDeviceQueue(logical_device, queue_family, 0, &result[i]);
	}
	return result;
}

VulkanContext::VulkanContext(ContextBuilder builder)
{
	instance = create_instance(builder.instance_layers, builder.instance_extensions);
	physical_device = select_physical_device(instance, builder.gpu_score);
	queue_family = select_queue_family_index(physical_device, builder.queue_score);
	logical_device = create_device(physical_device, queue_family, builder.queue_priorities, builder.device_extensions);
	command_pool = create_command_pool(logical_device, queue_family);
	execute_buffer = create_command_buffer(logical_device, command_pool);
	query_pool = create_query_pool(logical_device, 2);
	descriptor_pool = create_default_descriptor_pool();
	queues = get_queue_handles(logical_device, queue_family, builder.queue_priorities.size());
	execute_queue = queues.back();
}

float VulkanContext::execute(std::function<void(VkCommandBuffer)> command)
{
	with_command_buffer(execute_buffer, [&](VkCommandBuffer buffer) {
		vkCmdResetQueryPool(buffer, query_pool, 0, 2);
		vkCmdWriteTimestamp(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, query_pool, 0);
		command(buffer);
		vkCmdWriteTimestamp(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, query_pool, 1);
	});
	VkCommandBuffer buffer = execute_buffer;
	VkSubmitInfo info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &buffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};
	CHECK_VULKAN(vkQueueSubmit(execute_queue, 1, &info, nullptr));
	CHECK_VULKAN(vkQueueWaitIdle(execute_queue));
	uint64_t ts[2];
	CHECK_VULKAN(vkGetQueryPoolResults(logical_device, query_pool, 0, 2, sizeof(uint64_t) * 2, ts, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT));
	return (ts[1] - ts[0]);
}

void VulkanContext::with_command_buffer(VkCommandBuffer buffer, std::function<void(VkCommandBuffer)> closure)
{
	CHECK_VULKAN(vkResetCommandBuffer(buffer, 0));
	VkCommandBufferBeginInfo info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pInheritanceInfo = nullptr
	};
	CHECK_VULKAN(vkBeginCommandBuffer(buffer, &info));
	closure(buffer);
	CHECK_VULKAN(vkEndCommandBuffer(buffer));
}

Wrapper<VkDescriptorPool> VulkanContext::create_default_descriptor_pool()
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

	return create_descriptor_pool(logical_device, pool_sizes);
}

VulkanContext::operator VkDevice() const
{
	return logical_device;
}

VkInstance VulkanContext::get_instance()
{
	return instance;
}

VkPhysicalDevice VulkanContext::get_physical_device()
{
	return physical_device;
}

std::vector<VkQueue> VulkanContext::get_queues()
{
	return queues;
}

uint32_t VulkanContext::get_queue_family()
{
	return queue_family;
}

VkDescriptorPool VulkanContext::get_descriptor_pool()
{
	return descriptor_pool;
}

VkCommandPool VulkanContext::get_command_pool()
{
	return command_pool;
}

}
