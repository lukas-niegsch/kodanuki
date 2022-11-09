#include "plugin/vulkan/buffer.h"
#include "plugin/vulkan/debug.h"
#include <cstring>

namespace Kodanuki
{

class SimpleVulkanBuffer
{
public:
	SimpleVulkanBuffer(BufferBuilder builder)
		: device(builder.device)
		, number_of_bytes(builder.byte_size)
	{
		create_buffer(builder);
		allocate_buffer_memory();
		vkBindBufferMemory(device, buffer, memory, 0);
		fill_buffer_data(builder);
	}

	void destroy()
	{
		vkDestroyBuffer(device, buffer, nullptr);
		vkFreeMemory(device, memory, nullptr);
	}

	operator VkBuffer()
	{
		return buffer;
	}

	uint32_t byte_size()
	{
		return number_of_bytes;
	}

private:
	void create_buffer(BufferBuilder builder)
	{
		VkBufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.size = builder.byte_size;
		info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount =  0;
		info.pQueueFamilyIndices = nullptr;
		CHECK_VULKAN(vkCreateBuffer(device, &info, nullptr, &buffer));
	}

	void allocate_buffer_memory()
	{
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device, buffer, &requirements);
		uint32_t memory_type = find_memory_type(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkMemoryAllocateInfo allocate_info = {};
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.pNext = nullptr;
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = memory_type;

		CHECK_VULKAN(vkAllocateMemory(device, &allocate_info, nullptr, &memory));
	}

	void fill_buffer_data(BufferBuilder builder)
	{
		void* data;
		vkMapMemory(device, memory, 0, builder.byte_size, 0, &data);
		memcpy(data, builder.data, builder.byte_size);
		vkUnmapMemory(device, memory);
	}

	uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device.physical_device(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((type_filter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

private:
	VulkanDevice device;
	uint32_t number_of_bytes;
	VkBuffer buffer;
	VkDeviceMemory memory;
};

VulkanBuffer::VulkanBuffer(BufferBuilder builder)
{
	ECS::update<SimpleVulkanBuffer>(impl, {builder});
}

void VulkanBuffer::shared_destructor()
{
	ECS::get<SimpleVulkanBuffer>(impl).destroy();
}

VulkanBuffer::operator VkBuffer()
{
	return ECS::get<SimpleVulkanBuffer>(impl);
}

uint32_t VulkanBuffer::byte_size()
{
	return ECS::get<SimpleVulkanBuffer>(impl).byte_size();
}

}
