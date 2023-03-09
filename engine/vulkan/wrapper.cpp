#include "engine/vulkan/wrapper.h"
#include "engine/vulkan/debug.h"

namespace kodanuki
{

VulkanShader create_shader(VulkanDevice device, std::vector<char> code)
{
	return create_wrapper<vkCreateShaderModule, vkDestroyShaderModule>({
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data())
	}, device);
}

}
