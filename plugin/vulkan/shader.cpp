#include "plugin/vulkan/shader.h"
#include "plugin/vulkan/debug.h"

namespace Kodanuki
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

struct VulkanShaderInfo
{
	std::string entry_point;
};

VulkanShader::VulkanShader(ShaderBuilder builder)
{
	ECS::update<VulkanDevice>(impl, builder.device);
	ECS::update<VkShaderModule>(impl, create_shader_module(builder));
	ECS::update<VulkanShaderInfo>(impl, {builder.entry_point});
}

void VulkanShader::shared_destructor()
{
	VulkanDevice device = ECS::get<VulkanDevice>(impl);
	vkDestroyShaderModule(device, shader_module(), nullptr);
}

VkShaderModule VulkanShader::shader_module()
{
	return ECS::get<VkShaderModule>(impl);
}

std::string VulkanShader::entry_point()
{
	return ECS::get<VulkanShaderInfo>(impl).entry_point;
}

}
