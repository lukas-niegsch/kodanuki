#include "example.h"
#include "plugin/vulkan/debug.h"

VkRenderPass ExamplePipelineInfo::get_renderpass(VulkanDevice device, VulkanSwapchain swapchain)
{
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription color_attachment = {};
	color_attachment.format = swapchain.surface_format().format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_reference = {};
	color_attachment_reference.attachment = 0;
	color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_reference;

	VkRenderPassCreateInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_info.attachmentCount = 1;
	renderpass_info.pAttachments = &color_attachment;
	renderpass_info.subpassCount = 1;
	renderpass_info.pSubpasses = &subpass;
	renderpass_info.dependencyCount = 1;
	renderpass_info.pDependencies = &dependency;

	VkRenderPass renderpass;
	CHECK_VULKAN(vkCreateRenderPass(device.logical_device(), &renderpass_info, nullptr, &renderpass));
	return renderpass;
}

VkPipelineDynamicStateCreateInfo ExamplePipelineInfo::get_dynamic_state()
{
	states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = static_cast<uint32_t>(states.size());
	dynamic_state.pDynamicStates = states.data();
	return dynamic_state;
}

VkPipelineVertexInputStateCreateInfo ExamplePipelineInfo::get_vertex_input()
{
	VkPipelineVertexInputStateCreateInfo vertex_input = {};
	vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input.vertexBindingDescriptionCount = 0;
	vertex_input.pVertexBindingDescriptions = nullptr;
	vertex_input.vertexAttributeDescriptionCount = 0;
	vertex_input.pVertexAttributeDescriptions = nullptr;
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

VkPipelineViewportStateCreateInfo ExamplePipelineInfo::get_viewport()
{
	VkPipelineViewportStateCreateInfo viewport = {};
	viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport.viewportCount = 1;
	viewport.pViewports = nullptr;
	viewport.scissorCount = 1;
	viewport.pScissors = nullptr;
	return viewport;
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
