
#include "source/splash/model.h"
#include "engine/vulkan/utility.h"
#include "source/splash/shader_bridge.h"
#define GLM_FORCE_RADIANS
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

void create_simple_renderpass(VulkanDevice device, VkRenderPass& renderpass)
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

void handle_user_inputs(Config& config, float dts, uint32_t frame, VulkanWindow window, VulkanTarget target, ShaderBridge& bridge, glm::vec3& player_position, glm::vec3& player_rotation)
{
	glm::vec3 rotation {0.0f};
	
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
	if (window.is_key_pressed(GLFW_KEY_LEFT)) {
		rotation.y += 1.0f;
	}
	if (window.is_key_pressed(GLFW_KEY_RIGHT)) {
		rotation.y -= 1.0f;
	}
	if (window.is_key_pressed(GLFW_KEY_UP)) {
		rotation.x += 1.0f;
	}
	if (window.is_key_pressed(GLFW_KEY_DOWN)) {
		rotation.x -= 1.0f;
	}

	if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon()) {
		player_rotation += config.look_speed * dts * glm::normalize(rotation); 
	}
	player_rotation.x = glm::clamp(player_rotation.x, glm::degrees(-75.0f), glm::degrees(75.0f));
	player_rotation.y = glm::mod(player_rotation.y, glm::degrees(glm::two_pi<float>()));

	float yaw = player_rotation.y;
	float pitch = player_rotation.x;

	glm::vec3 forward = glm::normalize(glm::vec3{
		glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
		glm::sin(glm::radians(pitch)),
		glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))
	});
	glm::vec3 world_up {0.0f, -1.0f, 0.0f};
	glm::vec3 right = glm::normalize(glm::cross(forward, world_up));
	glm::vec3 up    = glm::normalize(glm::cross(right, forward));

	glm::vec3 position {0.0f};
	
	if (window.is_key_pressed(GLFW_KEY_A)) {
		position -= right;
	}
	if (window.is_key_pressed(GLFW_KEY_S)) {
		position -= forward;
	}
	if (window.is_key_pressed(GLFW_KEY_D)) {
		position += right;
	}
	if (window.is_key_pressed(GLFW_KEY_W)) {
		position += forward;
	}
	if (window.is_key_pressed(GLFW_KEY_E)) {
		position -= up;
	}
	if (window.is_key_pressed(GLFW_KEY_Q)) {
		position += up;
	}

	if (glm::dot(position, position) > std::numeric_limits<float>::epsilon()) {
		player_position += config.move_speed * dts * glm::normalize(position); 
	}

	auto buffer = bridge.get_buffer_mvp(frame);
	auto extent = target.get_surface_extent();
	buffer[0].model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 1.0f));
	buffer[0].view = glm::lookAt(player_position, player_position + forward, world_up);
	buffer[0].projection = glm::perspective(glm::radians(45.0f), extent.width / (float) extent.height, 0.1f, config.render_distance);
	buffer.slow_push();
}

}
