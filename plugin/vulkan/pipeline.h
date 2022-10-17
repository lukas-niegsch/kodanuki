#pragma once
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/shader.h"
#include "engine/central/entity.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Kodanuki
{

/**
 * Contains all the configurable information for creating a pipeline.
 *
 * These values will be considered as much as possible when creating
 * the graphics pipeline. The caller has to make sure that the provided
 * values make sense.
 *
 * Example:
 * The shader must be formatted correctly.
 */
struct PipelineBuilder
{
	// The vulkan device for which the pipeline is created.
	VulkanDevice device;

	// The handle to the vertex shader module.
	std::optional<VulkanShader> vertex_shader;

	// The handle to the tesselation shader module.
	std::optional<VulkanShader> tesselation;

	// The handle to the geometry shader module.
	std::optional<VulkanShader> geometry_shader;

	// The handle to the fragment shader module.
	std::optional<VulkanShader> fragment_shader;
};

/**
 * The vulkan pipeline is a wrapper around the pipeline and render pass.
 *
 * Each vulkan pipeline creates a pipeline and render pass from the given
 * builder and handles the iteration with them. Pipelines must be recreated
 * manually for every changes in the shader.
 * 
 * Instances can be copied around freely and will release all
 * ressources once unused.
 */
class VulkanPipeline
{
public:
	// Creates a new vulkan pipeline from the given builder.
	VulkanPipeline(PipelineBuilder builder);

private:
	// Destroys unused pipelines automatically.
	std::shared_ptr<Entity> pimpl;
};

}
