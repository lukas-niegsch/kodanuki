// ----------------------------------------------------------------------------
// ------ Common CPP Code -----------------------------------------------------
// ----------------------------------------------------------------------------
#include "engine/vulkan/memory.h"
#include "engine/vulkan/debug.h"
#include "engine/vulkan/device.h"
#include "engine/vulkan/utility.h"

namespace kodanuki
{

void create_internal_buffer(VulkanDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory)
{
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	CHECK_VULKAN(vkCreateBuffer(device, &buffer_info, nullptr, &buffer));

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

	VkMemoryAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.allocationSize = memory_requirements.size;
	allocate_info.memoryTypeIndex = find_memory_type(device, memory_requirements.memoryTypeBits, properties);
	CHECK_VULKAN(vkAllocateMemory(device, &allocate_info, nullptr, &memory));
	vkBindBufferMemory(device, buffer, memory, 0);
}

MemoryState::~MemoryState()
{
	vkDestroyBuffer(device, memory_buffer, nullptr);
	vkFreeMemory(device, memory, nullptr);
}

VulkanMemory::VulkanMemory(MemoryBuilder builder)
{
	state = std::make_shared<MemoryState>(builder.device);
	state->byte_size = builder.byte_size;
	
	create_internal_buffer(
		state->device,
		builder.byte_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		state->memory_buffer,
		state->memory
	);
};

VulkanMemory::operator VkBuffer()
{
	return state->memory_buffer;
}

uint32_t VulkanMemory::get_byte_size()
{
	return state->byte_size;
}

StagingBufferState::~StagingBufferState()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	vkUnmapMemory(device, staging_memory);
	vkDestroyBuffer(device, staging_buffer, nullptr);
	vkFreeMemory(device, staging_memory, nullptr);
	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
	vkDestroyCommandPool(device, command_pool, nullptr);
}

}
