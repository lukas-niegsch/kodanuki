#pragma once
#include "engine/graphics/window.h"
#include "engine/framework/game.h"
#include "engine/framework/module.h"
#include "engine/concept/polygon.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Kodanuki
{

/**
 * This module creates and manages a vulkan renderer.
 * 
 * Currently all components are combined in this class
 * for convinience since no architecture is chosen yet.
 */
class RenderModule : public Module
{
public:
	// Constructs the new render module.
	RenderModule(GameInfo& info, std::shared_ptr<WindowModule> window);

	// Called once when the module is attached.
	virtual void attach(Family context) override;

	// Called once when the module is detached.
	virtual void detach(Family context) override;

	// Called repeatedly to render the module.
	virtual void update(Family context) override;

private:
	void createInstance();
	void createSurface();
	bool isDeviceSuitable(VkPhysicalDevice device);
	void pickPhysicalDevice();
	bool isQueueFamilySuitable(VkQueueFamilyProperties family);
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffer();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void createSyncObjects();
	void submitCommandBuffer(VkCommandBuffer commandBuffer);
	void presentImage(uint32_t imageIndex);
	void createVertexBuffer(VkBuffer* result, uint64_t size);

private:
	GameInfo& info;
	std::shared_ptr<WindowModule> window;
	Polygon example;
	VkBuffer vertexBuffer;
	VkBuffer colorBuffer;

	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue presentQueue;
	VkQueue graphicsQueue;
	VkSwapchainKHR swapchain;
	VkRenderPass renderPass;
	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	uint32_t queueCount;
	uint32_t queueFamilyIndex;
	VkFormat imageFormat;
	VkExtent2D imageExtent;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;
};

}
