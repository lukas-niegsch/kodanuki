#pragma once
#include "engine/vulkan/device.h"
#include "engine/template/copyable.h"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Kodanuki
{

/**
 * Contains all the configurable information for creating a shader.
 *
 * The user has to make sure that these values make sense.
 *
 * Example:
 * The shader must be formatted correctly.
 */
struct ShaderBuilder
{
	// The vulkan device for which the shader is created.
	VulkanDevice device;

	// The code content of the shader.
	std::vector<char> code;

	// The entry point of the shader.
	std::string entry_point;
};

/**
 * The vulkan shader is a wrapper around the shader module.
 * 
 * Each vulkan shader creates a shader module and manages its destruction.
 * They do not contain any information for which stage the shader is used.
 */
class VulkanShader : public Copyable<VulkanShader>
{
public:
	// Creates a new vulkan shader from the given builder.
	VulkanShader(ShaderBuilder builder);

public:
	// Returns the handle to the shader module.
	VkShaderModule shader_module();

	// Returns the entry point of the shader.
	std::string entry_point();

private:
	// Called once all shaders copies are unused.
	void shared_destructor();
	friend class Copyable<VulkanShader>;
};

}
