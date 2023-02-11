#include "engine/vulkan/shader.h"
#include "engine/vulkan/debug.h"

namespace kodanuki
{

VkShaderModule create_shader_module(ShaderBuilder builder)
{
	VkShaderModuleCreateInfo shader_module_info;
	shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_info.pNext = nullptr;
	shader_module_info.flags = 0;
	shader_module_info.codeSize = builder.code.size();
	shader_module_info.pCode = reinterpret_cast<const uint32_t*>(builder.code.data());

	VkShaderModule result;
	CHECK_VULKAN(vkCreateShaderModule(builder.device, &shader_module_info, nullptr, &result));
	return result;
}

struct ShaderState
{
	VulkanDevice device;
	VkShaderModule shader_module;
	std::string entry_point;
	~ShaderState();
};

ShaderState::~ShaderState()
{
	vkDestroyShaderModule(device, shader_module, nullptr);
}

VulkanShader::VulkanShader(ShaderBuilder builder)
{
	auto shader_module = create_shader_module(builder);
	pimpl = std::make_shared<ShaderState>(builder.device, shader_module, builder.entry_point);
}

VulkanShader::operator VkShaderModule()
{
	return pimpl->shader_module;
}

std::string& VulkanShader::entry_point()
{
	return pimpl->entry_point;
}

}
