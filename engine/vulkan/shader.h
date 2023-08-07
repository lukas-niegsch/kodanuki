#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/wrapper.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <string>
#include <vector>

// Forward declaration to avoid namespace polution.
namespace spv_reflect { class ShaderModule; }

namespace kodanuki
{

/**
 * The vulkan shader stores information about shaders.
 *
 * It parses the given shader source code in SPIRV format and
 * provides useful information to the caller. The extension of
 * the shader files matters for determining the stage.
 */
class VulkanShader
{
public:
	/**
	 * Creates the vulkan shader from the given file.
	 *
	 * It will load the source code of the shader and parses as much
	 * information as possible. The shader stage is determined by the
	 * extension, e.g. example.vert.spv.
	 *
	 * @param device The device on which to create the layouts.
	 * @param file_path The file of the shader in SPIR-V format.
	 */
	VulkanShader(VulkanDevice device, const char * file_path);

	/**
	 * Returns the reference to the underlying shader module.
	 */
	operator VkShaderModule();

public:
	/**
	 * @return The entry point of the shader.
	 */
	std::string get_entry_point() const;

	/**
	 * @return The stage in which the shader is used.
	 */
	VkShaderStageFlagBits get_stage() const;

	/**
	 * @return The layout of the shaders descriptor sets.
	 */
	Wrapper<VkDescriptorSetLayout> get_descriptor_layout() const;

	/**
	 * @return The push constant range of the shader.
	 */
	VkPushConstantRange get_push_constant_range() const;

private:
	VulkanDevice device;
	Wrapper<VkShaderModule> shader_module;
	std::shared_ptr<spv_reflect::ShaderModule> reflect_module;
};

}
