#include "source/splash/vulkan_config.h"
#include "source/splash/model.h"
#include "engine/vulkan/debug.h"
#include "engine/central/utility/file.h"
#include "engine/central/utility/alignment.h"

namespace kodanuki
{

int score_physical_device(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

int score_queue_family(VkQueueFamilyProperties family)
{
	int score = family.queueCount;
	score *= family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	return score;
}

void create_simple_renderpass(VkDevice device, VkRenderPass& renderpass)
{
	// Our pipeline outputs a single color for each pixel.
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
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

	// And additional an depth value for each pixel for depth testing.
	VkAttachmentDescription depth_attachment = {};
	depth_attachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_reference = {};
	depth_attachment_reference.attachment = 1;
	depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// We have one subpass that corresponds to all the rendering.
	VkSubpassDescription render_subpass = {};
	render_subpass.flags = 0;
	render_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	render_subpass.inputAttachmentCount = 0;
	render_subpass.pInputAttachments = nullptr;
	render_subpass.colorAttachmentCount = 1;
	render_subpass.pColorAttachments = &color_attachment_reference;
	render_subpass.pResolveAttachments = nullptr;
	render_subpass.pDepthStencilAttachment = &depth_attachment_reference;
	render_subpass.preserveAttachmentCount = 0;
	render_subpass.pPreserveAttachments = nullptr;

	// We also have one subpass that corresponds to the computing.
	VkSubpassDescription compute_subpass = {};
	compute_subpass.flags = 0;
	compute_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
	compute_subpass.inputAttachmentCount = 0;
	compute_subpass.pInputAttachments = nullptr;
	compute_subpass.colorAttachmentCount = 0;
	compute_subpass.pColorAttachments = nullptr;
	compute_subpass.pResolveAttachments = nullptr;
	compute_subpass.pDepthStencilAttachment = nullptr;
	compute_subpass.preserveAttachmentCount = 0;
	compute_subpass.pPreserveAttachments = nullptr;

	// The dependencies the compute shader needs.
    VkSubpassDependency compute_dependency = {};
    compute_dependency.srcSubpass = 0;
    compute_dependency.dstSubpass = 1;
    compute_dependency.srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    compute_dependency.dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    compute_dependency.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    compute_dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	compute_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Introduce synchronization at the color attachment stage (join at end).
	VkSubpassDependency render_dependency = {};
	render_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	render_dependency.dstSubpass = 0;
	render_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	render_dependency.srcAccessMask = 0;
	render_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	render_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	render_dependency.dependencyFlags = 0;

	// Create the actual renderpass.
	std::vector<VkAttachmentDescription> attachments = {color_attachment, depth_attachment};
	std::vector<VkSubpassDescription> subpasses = {render_subpass};
	std::vector<VkSubpassDependency> dependencies = {render_dependency}; 
	(void) compute_subpass; (void) compute_dependency; 

	VkRenderPassCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.attachmentCount = static_cast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	info.subpassCount = static_cast<uint32_t>(subpasses.size());
	info.pSubpasses = subpasses.data();
	info.dependencyCount = static_cast<uint32_t>(dependencies.size());
	info.pDependencies = dependencies.data();
	CHECK_VULKAN(vkCreateRenderPass(device, &info, nullptr, &renderpass));
}

VulkanPipeline create_render_fluid_pipeline(VulkanDevice device, VulkanTarget target)
{
	// Don't include any dynamic states.
	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.pNext = nullptr;
	dynamic_state.flags = 0;
	dynamic_state.dynamicStateCount = 0;
	dynamic_state.pDynamicStates = nullptr;

	// We want to use uniform MVP matrices. Se we need to describe them.
	VkDescriptorSetLayoutBinding mpv_layout_binding = {};
    mpv_layout_binding.binding = 0;
    mpv_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    mpv_layout_binding.descriptorCount = 1;
    mpv_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    mpv_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo mvp_layout_info = {};
    mvp_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    mvp_layout_info.pNext = nullptr;
    mvp_layout_info.flags = 0;
    mvp_layout_info.bindingCount = 1;
    mvp_layout_info.pBindings = &mpv_layout_binding;

	// Next create the actual description layout for the MVP matrices.
	VkDescriptorSetLayout mvp_descriptor;
	CHECK_VULKAN(vkCreateDescriptorSetLayout(device, &mvp_layout_info, nullptr, &mvp_descriptor));

	// Lastly specify that we actually want to the mvp matrices.
	VkPipelineLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.pNext = nullptr;
    layout_info.flags = 0;
    layout_info.setLayoutCount = 1;
    layout_info.pSetLayouts = &mvp_descriptor;
    layout_info.pushConstantRangeCount = 0;
    layout_info.pPushConstantRanges = nullptr;

	// We want to use instanced rendering for the vertices.
	std::vector<VkVertexInputBindingDescription> vertex_input_bindings;
	vertex_input_bindings.push_back({0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX});
	vertex_input_bindings.push_back({1, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_INSTANCE});

	std::vector<VkVertexInputAttributeDescription> vertex_input_descriptions;
	vertex_input_descriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
	vertex_input_descriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
	vertex_input_descriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
	vertex_input_descriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
	vertex_input_descriptions.push_back({4, 1, VK_FORMAT_R32G32B32_SFLOAT, 0});

	// Allow input that corresponds to vertex data (position, instance color, instance position).
	VkPipelineVertexInputStateCreateInfo vertex_input = {};
	vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input.pNext = nullptr;
	vertex_input.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_bindings.size());
	vertex_input.pVertexBindingDescriptions = vertex_input_bindings.data();
	vertex_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_descriptions.size());;
	vertex_input.pVertexAttributeDescriptions = vertex_input_descriptions.data();

	// Allow input to be given as triangle lists.
	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.pNext = nullptr;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = false;

	// Don't clamp, cull faces that are counter clockwise (back faces).
	VkPipelineRasterizationStateCreateInfo resterization = {};
	resterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	resterization.pNext = nullptr;
	resterization.depthClampEnable = VK_FALSE;
	resterization.rasterizerDiscardEnable = VK_FALSE;
	resterization.polygonMode = VK_POLYGON_MODE_FILL;
	resterization.cullMode = VK_CULL_MODE_BACK_BIT;
	resterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	resterization.depthBiasEnable = VK_FALSE;
	resterization.depthBiasConstantFactor = 0.0f;
	resterization.depthBiasClamp = 0.0f;
	resterization.depthBiasSlopeFactor = 0.0f;
	resterization.lineWidth = 1.0f;

	// Combines RGB between two colors depending on their alpha value.
	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	color_blend_attachment.blendEnable = VK_TRUE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;

	// Simple color blending that allows for transparency.
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

	VkExtent2D surface_extent = target.get_surface_extent();
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = surface_extent.width;
	viewport.height = surface_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = surface_extent;

	// Always render the full vieport, don't cut anything away.
	VkPipelineViewportStateCreateInfo viewport_info = {};
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.pNext = nullptr;
	viewport_info.viewportCount = 1;
	viewport_info.pViewports = &viewport;
	viewport_info.scissorCount = 1;
	viewport_info.pScissors = &scissor;

	// Always use depth testing.
	VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.pNext = nullptr;
    depth_stencil.flags = 0;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = {};
    depth_stencil.back = {};
    depth_stencil.minDepthBounds = 0.0f;
    depth_stencil.maxDepthBounds = 1.0f;

	// Don't use multisampling.
	VkPipelineMultisampleStateCreateInfo multisample = {};
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.pNext = nullptr;
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample.sampleShadingEnable = VK_FALSE;
	multisample.minSampleShading = 1.0f;
	multisample.pSampleMask = nullptr;
	multisample.alphaToCoverageEnable = VK_FALSE;
	multisample.alphaToOneEnable = VK_FALSE;

	GraphicsPipelineBuilder builder = {
		.device = device,
		.target = target,
		.layout_info = layout_info,
		.dynamic_state = dynamic_state,
		.vertex_input = vertex_input,
		.input_assembly = input_assembly,
		.resterization = resterization,
		.color_blend = color_blend,
		.viewport = viewport_info,
		.multisample = multisample,
		.depth_stencil = depth_stencil,
		.descriptor_sets = {mvp_descriptor},
		.shaders = {
			VulkanShader(device, "assets/shaders/fluid.vert.spv"),
			VulkanShader(device, "assets/shaders/fluid.frag.spv")
		}
	};

	return VulkanPipeline(builder);
}

}
