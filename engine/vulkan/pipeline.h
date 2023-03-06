#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/shader.h"
#include "engine/vulkan/target.h"
#include <vulkan/vulkan.h>
#include <optional>

namespace kodanuki
{

/**
 * Contains all the configurable information for creating a graphics pipeline.
 *
 * These values will be considered as much as possible when creating
 * the graphics pipeline. The caller has to make sure that the provided
 * values make sense.
 *
 * Example:
 * The shader must be formatted correctly.
 * Input attachments and shader's must match.
 */
struct GraphicsPipelineBuilder
{
	// The vulkan device for which the pipeline is created.
	VulkanDevice device;

	// The target that the pipeline should use.
	VulkanTarget target;

	// The layout of the pipeline.
	VkPipelineLayoutCreateInfo layout_info;

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

	// The configuration of the depth stencil state;
	VkPipelineDepthStencilStateCreateInfo depth_stencil;

	// The owned descriptor sets that the pipeline owns.
	std::vector<VkDescriptorSetLayout> descriptor_sets;
};

/**
 * Contains all the configurable information for creating a compute pipeline.
 *
 * These values will be considered as much as possible when creating
 * the compute pipeline. The caller has to make sure that the provided
 * values make sense.
 *
 * Example:
 * The compute shader must be formatted correctly.
 * Descriptor sets must match the bindings in the compute shader.
 */
struct ComputePipelineBuilder
{
	// The vulkan device for which the pipeline is created.
	VulkanDevice device;

	// The handle to the compute shader module.
	VulkanShader compute_shader;

	// The number of bytes inside the push_constant field.
	uint32_t push_constant_byte_size;

	// The bindings that the pipeline has.
	std::vector<VkDescriptorSetLayoutBinding> bindings;
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
	VulkanPipeline(GraphicsPipelineBuilder builder);

	// Creates a new vulkan pipeline from the given builder.
	VulkanPipeline(ComputePipelineBuilder builder);

	// Automatically creates a vulkan pipeline from the SPIRV file.
	static VulkanPipeline from_comp_file(VulkanDevice device, std::string filename);

	// Returns the handle to the native vulkan pipeline.
	operator VkPipeline();

public:
	// Returns the only used descriptor set layouts.
	VkDescriptorSetLayout get_descriptor_layout();

	// Returns the owned pipeline layout.
	VkPipelineLayout get_pipeline_layout();

	// Returns the descriptor for this pipeline.
	VkDescriptorSet get_primary_descriptor();

private:
	// The abstract pointer to the implementation.
	std::shared_ptr<struct PipelineState> pimpl;
};

// Caching that allows the pipelines to be stored for reuse.
using VulkanPipelineCache = std::unordered_map<std::string, VulkanPipeline>;

}
