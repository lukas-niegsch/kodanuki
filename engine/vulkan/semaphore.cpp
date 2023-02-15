#include "engine/vulkan/semaphore.h"
#include "engine/vulkan/debug.h"

namespace kodanuki
{

struct SemaphoreState
{
	VulkanDevice device;
	VkSemaphore semaphore;
	~SemaphoreState();
};

SemaphoreState::~SemaphoreState()
{
	vkDestroySemaphore(device, semaphore, nullptr);
}

VulkanSemaphore::VulkanSemaphore(SemaphoreBuilder builder)
{
	state = std::make_shared<SemaphoreState>(builder.device);

	VkSemaphoreCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	CHECK_VULKAN(vkCreateSemaphore(state->device, &info, nullptr, &state->semaphore));
}

VulkanSemaphore::operator VkSemaphore()
{
	return state->semaphore;
}

}
