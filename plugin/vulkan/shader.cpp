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
	CHECK_VULKAN(vkCreateShaderModule(builder.device.logical_device(), &shader_module_info, nullptr, &result));
	return result;
}

void remove_shader(Entity* shader)
{
	VulkanDevice device = ECS::get<VulkanDevice>(*shader);
	VkShaderModule shader_module = ECS::get<VkShaderModule>(*shader);
	vkDestroyShaderModule(device.logical_device(), shader_module, nullptr);
	ECS::remove<Entity>(*shader);
	delete shader;
}

struct VulkanShaderInfo
{
	std::string entry_point;
};

VulkanShader::VulkanShader(ShaderBuilder builder)
{
	pimpl = std::shared_ptr<Entity>(new Entity, &remove_shader);
	Entity shader = *pimpl = ECS::create();

	VkShaderModule shader_module = create_shader_module(builder);
	VulkanShaderInfo info = {builder.entry_point};

	ECS::update<VulkanDevice>(shader, builder.device);
	ECS::update<VkShaderModule>(shader, shader_module);
	ECS::update<VulkanShaderInfo>(shader, info);
}

VkShaderModule VulkanShader::shader_module()
{
	return ECS::get<VkShaderModule>(*pimpl);
}

std::string VulkanShader::entry_point()
{
	return ECS::get<VulkanShaderInfo>(*pimpl).entry_point;
}

}
