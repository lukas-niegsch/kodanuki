#pragma once
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/shader.h"
#include "plugin/vulkan/renderpass.h"
#include "engine/central/entity.h"
#include "engine/template/copyable.h"
#include <vulkan/vulkan.h>
#include <optional>

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
 * Input attachments and shader's must match.
 */
struct PipelineBuilder
{
	// The vulkan device for which the pipeline is created.
	VulkanDevice device;

	// The renderpass that the pipeline should use.
	VulkanRenderpass renderpass;

	// The handle to the vertex shader module.
	std::optional<VulkanShader> vertex_shader;

	// The handle to the tesselation shader module.
	std::optional<VulkanShader> tesselation;

	// The handle to the geometry shader module.
	std::optional<VulkanShader> geometry_shader;

	// The handle to the fragment shader module.
	std::optional<VulkanShader> fragment_shader;
	
	// The configuration of the dynamic state.
	VkPipelineDynamicStateCreateInfo dynamic_state;

	// The configuration of the vertex input state.
	VkPipelineVertexInputStateCreateInfo vertex_input;

	// The configuration of the input assembly state.
	VkPipelineInputAssemblyStateCreateInfo input_assembly;

	// The configuration of the rasterization state.
	VkPipelineRasterizationStateCreateInfo resterization;

	// The configuration of the color blend state.
	VkPipelineColorBlendStateCreateInfo color_blend;

	// The configuration of the viewport state.
	VkPipelineViewportStateCreateInfo viewport;

	// The configuration of the multisample state.
	VkPipelineMultisampleStateCreateInfo multisample;
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
 *
 * TODO: @design Does the pipeline (own|includes|weakrefs) an renderpass?
 */
class VulkanPipeline : private Copyable<VulkanPipeline>
{
public:
	// Creates a new vulkan pipeline from the given builder.
	VulkanPipeline(PipelineBuilder builder);

public:
	// Returns the handle to the actual pipeline.
	VkPipeline pipeline();

private:
	// Called once all pipeline copies are unused.
	void shared_destructor();
	friend class Copyable<VulkanPipeline>;
};

}
