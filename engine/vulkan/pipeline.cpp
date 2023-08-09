#include "engine/vulkan/pipeline.h"
#include "engine/vulkan/debug.h"
#include "engine/central/utility/file.h"
#include "engine/vulkan/wrapper.h"
#include <cassert>

namespace kodanuki
{

void fill_shader_stages(std::vector<VkPipelineShaderStageCreateInfo>& stages, GraphicsPipelineBuilder& builder)
{
	for (VulkanShader shader : builder.shaders) {
		stages.push_back(shader.get_stage_create_info());
	}
}

std::pair<VkPipeline, VkPipelineLayout> create_graphics_pipeline(GraphicsPipelineBuilder builder)
{
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
	fill_shader_stages(shader_stages, builder);

	VkPipelineLayout layout;
	CHECK_VULKAN(vkCreatePipelineLayout(builder.device, &builder.layout_info, nullptr, &layout));

	VkGraphicsPipelineCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stageCount = shader_stages.size();
	info.pStages = shader_stages.data();
	info.pVertexInputState = &builder.vertex_input;
	info.pInputAssemblyState = &builder.input_assembly;
	info.pTessellationState = nullptr;
	info.pViewportState = &builder.viewport;
	info.pRasterizationState = &builder.resterization;
	info.pMultisampleState = &builder.multisample;
	info.pDepthStencilState = &builder.depth_stencil;
	info.pColorBlendState = &builder.color_blend;
	info.pDynamicState = &builder.dynamic_state;
	info.layout = layout;
	info.renderPass = builder.target.get_renderpass();
	info.subpass = 0;
	info.basePipelineHandle = VK_NULL_HANDLE;
	info.basePipelineIndex = -1;

	VkPipeline pipeline;
	CHECK_VULKAN(vkCreateGraphicsPipelines(builder.device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline));
	return {pipeline, layout};
}

struct PipelineState
{
	VulkanDevice device;
	VkPipeline pipeline;
	VkPipelineLayout layout;
	VkDescriptorSetLayout descriptor;
	Wrapper<VkDescriptorSetLayout> descriptor2;
	Wrapper<VkDescriptorSet> descriptor_set;
	~PipelineState();
};

PipelineState::~PipelineState()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	descriptor_set = {};
	vkDestroyPipeline(device, pipeline, nullptr);
	vkDestroyPipelineLayout(device, layout, nullptr);
	vkDestroyDescriptorSetLayout(device, descriptor, nullptr);
}

VulkanPipeline::VulkanPipeline(GraphicsPipelineBuilder builder)
{
	auto[pipeline, layout] = create_graphics_pipeline(builder);
	pimpl = std::make_shared<PipelineState>(builder.device);
	pimpl->descriptor = builder.descriptor_sets[0];
	pimpl->pipeline = pipeline;
	pimpl->layout = layout;
	pimpl->descriptor_set = create_descriptor_set(pimpl->device,
		pimpl->device.get_descriptor_pool(), pimpl->descriptor);
}

VulkanPipeline::VulkanPipeline(VulkanDevice device, VulkanShader shader)
{
	pimpl = std::make_shared<PipelineState>(device);
	VkDescriptorSetLayout layout = pimpl->descriptor2 = shader.get_descriptor_layout();
	VkPushConstantRange push_constant_range = shader.get_push_constant_range();

	VkPipelineLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layout_info.pNext = nullptr;
	layout_info.flags = 0;
	layout_info.setLayoutCount = 1;
	layout_info.pSetLayouts = &layout;
	layout_info.pushConstantRangeCount = 1;
	layout_info.pPushConstantRanges = &push_constant_range;
	CHECK_VULKAN(vkCreatePipelineLayout(device, &layout_info, nullptr, &pimpl->layout));

	VkComputePipelineCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stage = shader.get_stage_create_info();
	info.layout = pimpl->layout;
	info.basePipelineHandle = VK_NULL_HANDLE;
	info.basePipelineIndex = -1;
	CHECK_VULKAN(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pimpl->pipeline));
	
	pimpl->descriptor_set = create_descriptor_set(device,
		device.get_descriptor_pool(),
		pimpl->descriptor2);
}

VulkanPipeline::operator VkPipeline()
{
	return pimpl->pipeline;
}

VkDescriptorSetLayout VulkanPipeline::get_descriptor_layout()
{
	return pimpl->descriptor;
}

VkPipelineLayout VulkanPipeline::get_pipeline_layout()
{
	return pimpl->layout;
}

VkDescriptorSet VulkanPipeline::get_primary_descriptor()
{
	return pimpl->descriptor_set;
}

}
