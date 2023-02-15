#pragma once
#include "engine/vulkan/device.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace kodanuki
{

struct SemaphoreBuilder
{
	VulkanDevice device;
};

class VulkanSemaphore
{
public:
	VulkanSemaphore(SemaphoreBuilder builder);
	operator VkSemaphore();

private:
	std::shared_ptr<struct SemaphoreState> state;
};

}
