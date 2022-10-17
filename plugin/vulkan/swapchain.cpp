#include "plugin/vulkan/swapchain.h"

namespace Kodanuki
{

void remove_swapchain(Entity* swapchain)
{
    VulkanDevice device = ECS::get<VulkanDevice>(*swapchain);
    VkSurfaceKHR surface = ECS::get<VkSurfaceKHR>(*swapchain);

    vkDestroySurfaceKHR(device.instance(), surface, nullptr);
	ECS::remove<Entity>(*swapchain);
	delete swapchain;
}

VulkanSwapchain::VulkanSwapchain(SwapchainCreateInfo builder)
{
	pimpl = std::shared_ptr<Entity>(new Entity, &remove_swapchain);
	Entity swapchain = *pimpl = ECS::create();



    ECS::update<VulkanDevice>(swapchain, builder.device);
    ECS::update<VkSurfaceKHR>(swapchain, builder.surface);
}

VkSurfaceKHR VulkanSwapchain::surface()
{
    return ECS::get<VkSurfaceKHR>(*pimpl);
}

}
