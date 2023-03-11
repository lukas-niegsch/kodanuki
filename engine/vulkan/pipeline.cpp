#include "engine/vulkan/pipeline.h"
#include "engine/vulkan/debug.h"
#include "engine/utility/file.h"
#include "engine/vulkan/wrapper.h"
#include <cassert>

namespace kodanuki
{

VkPipelineShaderStageCreateInfo create_shader_stage(Wrapper<VkShaderModule> shader, VkShaderStageFlagBits bit)
{
	VkPipelineShaderStageCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.pNext = nullptr;
	info.stage = bit;
	info.module = shader;
	info.pName = "main";
	info.pSpecializationInfo = nullptr;
	return info;
}

void fill_shader_stages(std::vector<VkPipelineShaderStageCreateInfo>& stages, GraphicsPipelineBuilder& builder)
{
	auto append_stage = [&](std::optional<Wrapper<VkShaderModule>> shader, VkShaderStageFlagBits bit) {
		if (shader) {
			stages.push_back(create_shader_stage(shader.value(), bit));
		}
	};

	append_stage(builder.vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
	append_stage(builder.tesselation, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	append_stage(builder.geometry_shader, VK_SHADER_STAGE_GEOMETRY_BIT);
	append_stage(builder.fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);
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
	info.renderPass = builder.target.renderpass();
	info.subpass = 0;
	info.basePipelineHandle = VK_NULL_HANDLE;
	info.basePipelineIndex = -1;

	VkPipeline pipeline;
	CHECK_VULKAN(vkCreateGraphicsPipelines(builder.device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline));
	return {pipeline, layout};
}

struct PipelineState
{
	VulkanContext device;
	VkPipeline pipeline;
	VkPipelineLayout layout;
	VkDescriptorSetLayout descriptor;
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

VulkanPipelineOld::VulkanPipelineOld(GraphicsPipelineBuilder builder)
{
	auto[pipeline, layout] = create_graphics_pipeline(builder);
	pimpl = std::make_shared<PipelineState>(builder.device);
	pimpl->descriptor = builder.descriptor_sets[0];
	pimpl->pipeline = pipeline;
	pimpl->layout = layout;
	pimpl->descriptor_set = create_descriptor_set(pimpl->device,
		pimpl->device.get_descriptor_pool(), pimpl->descriptor);
}

VulkanPipelineOld::VulkanPipelineOld(ComputePipelineBuilder builder)
{
	pimpl = std::make_shared<PipelineState>(builder.device);
	
	VkDescriptorSetLayoutCreateInfo descriptor_layout_info = {};
	descriptor_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_layout_info.bindingCount = static_cast<uint32_t>(builder.bindings.size());
	descriptor_layout_info.pBindings = builder.bindings.data();
	CHECK_VULKAN(vkCreateDescriptorSetLayout(builder.device, &descriptor_layout_info, nullptr, &pimpl->descriptor));

	VkPushConstantRange push_constant_range = {};
	push_constant_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	push_constant_range.offset = 0;
	push_constant_range.size = builder.push_constant_byte_size;

	VkPipelineLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.pNext = nullptr;
    layout_info.flags = 0;
    layout_info.setLayoutCount = 1;
    layout_info.pSetLayouts = &pimpl->descriptor;
    layout_info.pushConstantRangeCount = 1;
    layout_info.pPushConstantRanges = &push_constant_range;
	CHECK_VULKAN(vkCreatePipelineLayout(builder.device, &layout_info, nullptr, &pimpl->layout));

	VkComputePipelineCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stage = create_shader_stage(builder.compute_shader, VK_SHADER_STAGE_COMPUTE_BIT);
	info.layout = pimpl->layout;
	info.basePipelineHandle = VK_NULL_HANDLE;
	info.basePipelineIndex = -1;
	CHECK_VULKAN(vkCreateComputePipelines(builder.device, VK_NULL_HANDLE, 1, &info, nullptr, &pimpl->pipeline));
	
	pimpl->descriptor_set = create_descriptor_set(pimpl->device,
		pimpl->device.get_descriptor_pool(),
		pimpl->descriptor);
}

VulkanPipelineOld VulkanPipelineOld::from_comp_file(VulkanContext device, std::string filename)
{
	std::vector<char> code = read_file_into_buffer(filename);
	
	SpvReflectShaderModule reflect_module;
	CHECK_SPIRV(spvReflectCreateShaderModule(code.size(), code.data(), &reflect_module));

	auto reflect_bindings = vectorize<spvReflectEnumerateDescriptorBindings>(&reflect_module);
	std::vector<VkDescriptorSetLayoutBinding> bindings(reflect_bindings.size());
	for (uint32_t i = 0; i < bindings.size(); i++) {
		const SpvReflectDescriptorBinding* spv_binding = reflect_bindings[i];
		VkDescriptorSetLayoutBinding& binding = bindings[i];
		binding.binding = spv_binding->binding;
		binding.descriptorType = static_cast<VkDescriptorType>(spv_binding->descriptor_type);
		binding.descriptorCount = spv_binding->count;
		binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		binding.pImmutableSamplers = nullptr;
	}

	auto push_constants = vectorize<spvReflectEnumeratePushConstantBlocks>(&reflect_module);
	assert(push_constants.size() == 1); // Only one push constant block allowed!

	ComputePipelineBuilder builder = {
		.device = device,
		.compute_shader = create_shader_module(device, code),
		.push_constant_byte_size = push_constants[0]->size,
		.bindings = bindings
	};

	spvReflectDestroyShaderModule(&reflect_module);
	return VulkanPipelineOld(builder);
}

VulkanPipelineOld::operator VkPipeline()
{
	return pimpl->pipeline;
}

VkDescriptorSetLayout VulkanPipelineOld::get_descriptor_layout()
{
	return pimpl->descriptor;
}

VkPipelineLayout VulkanPipelineOld::get_pipeline_layout()
{
	return pimpl->layout;
}

VkDescriptorSet VulkanPipelineOld::get_primary_descriptor()
{
	return pimpl->descriptor_set;
}

}
