#include "engine/vulkan/shader.h"
#include "engine/vulkan/debug.h"
#include "engine/central/utility/file.h"
#include "extern/SPIRV-Reflect/spirv_reflect.h"
#include <cassert>

namespace kodanuki
{

VulkanShader::VulkanShader(VulkanDevice device, const char * file_path) : device(device)
{
	std::vector<char> code = read_file_into_buffer(file_path);
	std::vector<uint8_t> code_bytes(code.begin(), code.end());
	reflect_module = std::make_shared<spv_reflect::ShaderModule>(code_bytes);
	shader_module = create_shader_module(device, code);
}

VulkanShader::operator VkShaderModule()
{
	return shader_module;
}

std::string VulkanShader::get_entry_point() const
{
	return reflect_module->GetEntryPointName();
}

VkShaderStageFlagBits VulkanShader::get_stage() const
{
	return static_cast<VkShaderStageFlagBits>(reflect_module->GetShaderStage());
}

Wrapper<VkDescriptorSetLayout> VulkanShader::get_descriptor_layout() const
{
	return create_descriptor_layout(device, reflect_module->GetShaderModule());
}

VkPushConstantRange VulkanShader::get_push_constant_range() const
{
	// TODO: remove default values
	VkPushConstantRange range = {
		.stageFlags = get_stage(),
		.offset = 0,
		.size = 128
	};
	return range;
}

VkPipelineShaderStageCreateInfo VulkanShader::get_stage_create_info() const
{
	return {
		.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext  = nullptr,
		.flags  = 0,
		.stage  = get_stage(),
		.module = shader_module,
		.pName  = "main",
		.pSpecializationInfo = nullptr
	};
}

}
