#pragma once
#include "engine/vulkan/device.h"
#include "engine/utility/wrapper.h"
#include <vulkan/vulkan.h>

namespace kodanuki
{

using VulkanShader = Wrapper<VkShaderModule>;

/**
 * Shader modules contain shader code that the device can execute.
 *
 * @param device The device that stores the shader.
 * @param code The bytes of the shader SPIRV shader file.
 * @return The wrapper around the vulkan shader module.
 */
VulkanShader create_shader(VulkanDevice device, std::vector<char> code);

}
