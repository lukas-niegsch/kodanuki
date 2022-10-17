#pragma once
#include "plugin/vulkan/device.h"
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <memory>
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
 *
 * Instances can be copied around freely and will release all
 * ressources once unused.
 */
class VulkanShader
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
	// Destroys unused shaders automatically.
	std::shared_ptr<Entity> pimpl;
};

}
