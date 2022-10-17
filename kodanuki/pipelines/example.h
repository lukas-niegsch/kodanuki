#pragma once
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/swapchain.h"
#include <vulkan/vulkan.h>
using namespace Kodanuki;

class ExamplePipelineInfo
{
public:
	VkRenderPass get_renderpass(VulkanDevice device, VulkanSwapchain swapchain);
	VkPipelineDynamicStateCreateInfo get_dynamic_state();
	VkPipelineVertexInputStateCreateInfo get_vertex_input();
	VkPipelineInputAssemblyStateCreateInfo get_input_assembly();
	VkPipelineRasterizationStateCreateInfo get_resterization();
	VkPipelineColorBlendStateCreateInfo get_color_blend();
	VkPipelineViewportStateCreateInfo get_viewport();
	VkPipelineMultisampleStateCreateInfo get_multisample();

private:
	std::vector<VkDynamicState> states;
	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
};
