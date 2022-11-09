#include "example.h"
#include "plugin/vulkan/debug.h"
#include "engine/utility/file.h"

class ExamplePipelineInfo
{
public:
	VkSubpassDependency get_dependency();
	VkAttachmentDescription get_color_attachment();
	VkSubpassDescription get_subpasses();
	VkPipelineDynamicStateCreateInfo get_dynamic_state();
	VkPipelineVertexInputStateCreateInfo get_vertex_input();
	VkPipelineInputAssemblyStateCreateInfo get_input_assembly();
	VkPipelineRasterizationStateCreateInfo get_resterization();
	VkPipelineColorBlendStateCreateInfo get_color_blend();
	VkPipelineViewportStateCreateInfo get_viewport(VulkanSwapchain swapchain);
	VkPipelineMultisampleStateCreateInfo get_multisample();

private:
	std::vector<VkDynamicState> states;
	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	VkAttachmentReference color_attachment_reference = {};
	VkViewport viewport = {};
	VkRect2D scissor = {};
	VkVertexInputBindingDescription vertex_input_binding;
	std::array<VkVertexInputAttributeDescription, 2> vertex_info_attributes;
};

VkSubpassDependency ExamplePipelineInfo::get_dependency()
{
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	return dependency;
}

VkAttachmentDescription ExamplePipelineInfo::get_color_attachment()
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: make this dynamic
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	return color_attachment;
}

VkSubpassDescription ExamplePipelineInfo::get_subpasses()
{
	color_attachment_reference.attachment = 0;
	color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_reference;
	return subpass;
}

VkPipelineDynamicStateCreateInfo ExamplePipelineInfo::get_dynamic_state()
{
	states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = 0; // static_cast<uint32_t>(states.size());
	dynamic_state.pDynamicStates = nullptr; // states.data();
	return dynamic_state;
}

VkPipelineVertexInputStateCreateInfo ExamplePipelineInfo::get_vertex_input()
{
	vertex_input_binding = Vertex::binding_description();
	vertex_info_attributes = Vertex::attribute_descriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input = {};
	vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input.vertexBindingDescriptionCount = 1;
	vertex_input.pVertexBindingDescriptions = &vertex_input_binding;
	vertex_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_info_attributes.size());;
	vertex_input.pVertexAttributeDescriptions = vertex_info_attributes.data();
	return vertex_input;
}

VkPipelineInputAssemblyStateCreateInfo ExamplePipelineInfo::get_input_assembly()
{
	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = false;
	return input_assembly;
}

VkPipelineRasterizationStateCreateInfo ExamplePipelineInfo::get_resterization()
{
	VkPipelineRasterizationStateCreateInfo resterization = {};
	resterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	resterization.depthClampEnable = VK_FALSE;
	resterization.rasterizerDiscardEnable = VK_FALSE;
	resterization.polygonMode = VK_POLYGON_MODE_FILL;
	resterization.cullMode = VK_CULL_MODE_BACK_BIT;
	resterization.frontFace = VK_FRONT_FACE_CLOCKWISE;
	resterization.depthBiasEnable = VK_FALSE;
	resterization.depthBiasConstantFactor = 0.0f;
	resterization.depthBiasClamp = 0.0f;
	resterization.depthBiasSlopeFactor = 0.0f;
	resterization.lineWidth = 1.0f;
	return resterization;
}

VkPipelineColorBlendStateCreateInfo ExamplePipelineInfo::get_color_blend()
{
	color_blend_attachment.blendEnable = VK_TRUE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;

	VkPipelineColorBlendStateCreateInfo color_blend = {};
	color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend.logicOpEnable = VK_FALSE;
	color_blend.logicOp = VK_LOGIC_OP_NO_OP;
	color_blend.attachmentCount = 1;
	color_blend.pAttachments = &color_blend_attachment;
	color_blend.blendConstants[0] = 0.0f;
	color_blend.blendConstants[1] = 0.0f;
	color_blend.blendConstants[2] = 0.0f;
	color_blend.blendConstants[3] = 0.0f;
	return color_blend;
}

VkPipelineViewportStateCreateInfo ExamplePipelineInfo::get_viewport(VulkanSwapchain swapchain)
{
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = swapchain.surface_extent().width;
	viewport.height = swapchain.surface_extent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	scissor.offset = {0, 0};
	scissor.extent = swapchain.surface_extent();

	VkPipelineViewportStateCreateInfo viewport_info = {};
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount = 1;
	viewport_info.pViewports = &viewport;
	viewport_info.scissorCount = 1;
	viewport_info.pScissors = &scissor;
	return viewport_info;
}

VkPipelineMultisampleStateCreateInfo ExamplePipelineInfo::get_multisample()
{
	VkPipelineMultisampleStateCreateInfo multisample = {};
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample.sampleShadingEnable = VK_FALSE;
	multisample.minSampleShading = 1.0f;
	multisample.pSampleMask = nullptr;
	multisample.alphaToCoverageEnable = VK_FALSE;
	multisample.alphaToOneEnable = VK_FALSE;
	return multisample;
}

VulkanRenderpass get_example_triangle_renderpass(VulkanDevice device)
{
	ExamplePipelineInfo example;

	RenderpassBuilder builder = {
		.device = device,
		.dependencies = {example.get_dependency()},
		.attachments = {example.get_color_attachment()},
		.subpasses = {example.get_subpasses()}
	};

	return VulkanRenderpass(builder);
}

VulkanPipeline get_example_triangle_pipeline(VulkanDevice device, VulkanSwapchain swapchain, VulkanRenderpass renderpass)
{
	ExamplePipelineInfo example;

	ShaderBuilder example_vertex_builder = {
		.device = device,
		.code = read_file_into_buffer("shader/example.vert.spv"),
		.entry_point = "main"
	};

	ShaderBuilder example_fragment_builder = {
		.device = device,
		.code = read_file_into_buffer("shader/example.frag.spv"),
		.entry_point = "main"
	};

	PipelineBuilder builder = {
		.device = device,
		.renderpass = renderpass,
		.vertex_shader = VulkanShader(example_vertex_builder),
		.tesselation = {},
		.geometry_shader = {},
		.fragment_shader = VulkanShader(example_fragment_builder),
		.dynamic_state = example.get_dynamic_state(),
		.vertex_input = example.get_vertex_input(),
		.input_assembly = example.get_input_assembly(),
		.resterization = example.get_resterization(),
		.color_blend = example.get_color_blend(),
		.viewport = example.get_viewport(swapchain),
		.multisample = example.get_multisample()
	};

	return VulkanPipeline(builder);	
}
