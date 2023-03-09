#include "engine/vulkan/shader.h"
#include "engine/vulkan/debug.h"

namespace kodanuki
{

struct ShaderState
{
	Wrapper<VkShaderModule> shader;
	std::string entry_point;
};

VulkanShader::VulkanShader(ShaderBuilder builder)
{
	pimpl = std::make_shared<ShaderState>();
	pimpl->entry_point = builder.entry_point;
	pimpl->shader = create_wrapper<vkCreateShaderModule, vkDestroyShaderModule>({
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = builder.code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(builder.code.data())
	}, builder.device);
}

VulkanShader::operator VkShaderModule()
{
	return pimpl->shader;
}

std::string& VulkanShader::entry_point()
{
	return pimpl->entry_point;
}

}
