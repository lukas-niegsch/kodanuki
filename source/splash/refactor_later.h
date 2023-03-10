
#include "source/splash/model.h"
#include "engine/vulkan/utility.h"
#include "source/splash/shader_bridge.h"
#include "source/splash/camera.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

void create_simple_renderpass(VulkanDeviceOld device, VkRenderPass& renderpass)
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

void handle_user_inputs(Config& config, float dts, uint32_t frame, VulkanWindow window, VulkanTarget target, ShaderBridge& bridge, Camera& camera)
{
	camera.MovementSpeed = config.move_speed;
	camera.MouseSensitivity = config.look_speed / 200;

	if (window.is_key_pressed(GLFW_KEY_M)) {
		config.is_demo_open = true;
	}
	if (window.is_key_pressed(GLFW_KEY_N)) {
		config.is_demo_open = false;
	}
	if (window.is_key_pressed(GLFW_KEY_C)) {
		config.is_menu_open = true;
	}
	if (window.is_key_pressed(GLFW_KEY_V)) {
		config.is_menu_open = false;
	}
	if (window.is_key_pressed(GLFW_KEY_A)) {
		camera.process_keyboard(Camera::LEFT, dts);
	}
	if (window.is_key_pressed(GLFW_KEY_S)) {
		camera.process_keyboard(Camera::BACKWARD, dts);
	}
	if (window.is_key_pressed(GLFW_KEY_D)) {
		camera.process_keyboard(Camera::RIGHT, dts);
	}
	if (window.is_key_pressed(GLFW_KEY_W)) {
		camera.process_keyboard(Camera::FORWARD, dts);
	}
	if (window.is_key_pressed(GLFW_KEY_E)) {
		camera.process_keyboard(Camera::DOWN, dts);
	}
	if (window.is_key_pressed(GLFW_KEY_Q)) {
		camera.process_keyboard(Camera::UP, dts);
	}

	auto extent = target.get_surface_extent();
	MVP new_mvp;
	new_mvp.model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 1.0f));
	new_mvp.view = camera.get_view_matrix();
	new_mvp.projection = camera.get_projection_matrix(extent.width, extent.height, config.render_distance);
	bridge.update_mvp(new_mvp, frame);
}

}
