#include "engine/vulkan/pipeline.h"
#include "engine/vulkan/debug.h"

namespace Kodanuki
{

// TODO: replace "main" with shader.entry_point() without segfaulting

void fill_shader_stage(VkPipelineShaderStageCreateInfo& shader_stage, VulkanShader shader, VkShaderStageFlagBits stage)
{
	shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage.pNext = nullptr;
	shader_stage.flags = 0;
	shader_stage.stage = stage;
	shader_stage.module = shader.shader_module();
	shader_stage.pName = "main";
	shader_stage.pSpecializationInfo = nullptr;
}

void fill_builder_shader_stages(std::vector<VkPipelineShaderStageCreateInfo>& shader_stages, PipelineBuilder builder)
{
	int next = 0;

	auto fill_stage = [&](std::optional<VulkanShader> shader, VkShaderStageFlagBits bit) {
		if (shader) {
			fill_shader_stage(shader_stages[next], shader.value(), bit);
			next++;
		}
	};

	fill_stage(builder.vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
	fill_stage(builder.tesselation, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	fill_stage(builder.geometry_shader, VK_SHADER_STAGE_GEOMETRY_BIT);
	fill_stage(builder.fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);
}

int get_number_of_shader_stages(PipelineBuilder builder)
{
	int count = 0;
	count += builder.vertex_shader.has_value();
	count += builder.tesselation.has_value();
	count += builder.geometry_shader.has_value();
	count += builder.fragment_shader.has_value();
	return count;
}

VkPipeline create_pipeline(PipelineBuilder& builder)
{
	int count = get_number_of_shader_stages(builder);
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages(count);
	fill_builder_shader_stages(shader_stages, builder);
	VkDevice device = builder.device;
	
	VkPipelineLayoutCreateInfo layout_info;
	layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layout_info.pNext = nullptr;
	layout_info.flags = 0;
	layout_info.setLayoutCount = 0;
	layout_info.pSetLayouts = nullptr;
	layout_info.pushConstantRangeCount = 0;
	layout_info.pPushConstantRanges = nullptr;

	VkPipelineLayout layout;
	CHECK_VULKAN(vkCreatePipelineLayout(device, &layout_info, nullptr, &layout));

	VkGraphicsPipelineCreateInfo pipeline_info;
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.pNext = nullptr;
	pipeline_info.flags = 0;
	pipeline_info.stageCount = shader_stages.size();
	pipeline_info.pStages = shader_stages.data();
	pipeline_info.pVertexInputState = &builder.vertex_input;
	pipeline_info.pInputAssemblyState = &builder.input_assembly;
	pipeline_info.pTessellationState = nullptr;
	pipeline_info.pViewportState = &builder.viewport;
	pipeline_info.pRasterizationState = &builder.resterization;
	pipeline_info.pMultisampleState = &builder.multisample;
	pipeline_info.pDepthStencilState = nullptr;
	pipeline_info.pColorBlendState = &builder.color_blend;
	pipeline_info.pDynamicState = &builder.dynamic_state;
	pipeline_info.layout = layout;
	pipeline_info.renderPass = builder.renderpass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = -1;

	VkPipeline pipeline;
	CHECK_VULKAN(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline));

	vkDestroyPipelineLayout(device, layout, nullptr);
	return pipeline;
}

VulkanPipeline::VulkanPipeline(PipelineBuilder builder)
{
	ECS::update<VulkanDevice>(impl, builder.device);
	ECS::update<VkPipeline>(impl, create_pipeline(builder));
}

void VulkanPipeline::shared_destructor()
{
	VkDevice device = ECS::get<VulkanDevice>(impl);
	vkDestroyPipeline(device, pipeline(), nullptr);
}

VkPipeline VulkanPipeline::pipeline()
{
	return ECS::get<VkPipeline>(impl);
}

VulkanPipeline::operator VkPipeline()
{
	return ECS::get<VkPipeline>(impl);
}

}
