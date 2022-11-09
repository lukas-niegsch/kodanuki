#pragma once
#include "plugin/vulkan/device.h"
#include "engine/template/copyable.h"
#include <vulkan/vulkan.h>

namespace Kodanuki
{

struct BufferBuilder
{
	VulkanDevice device;
	uint32_t byte_size;
	void* data;
};

class VulkanBuffer : public Copyable<VulkanBuffer>
{
public:
	// Creates a new vulkan buffer from the given builder.
	VulkanBuffer(BufferBuilder builder);

	// Called automatically once all instances are unused.
	void shared_destructor();

	// Returns the handle to the native vulkan buffer.
	operator VkBuffer();

public:
	uint32_t byte_size();
};

}
