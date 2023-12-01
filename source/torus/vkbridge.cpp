#include "source/torus/vkbridge.h"


int score_device_hardware(vktype::hardware_t hardware)
{
	int score = 0;
	score += hardware.physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	score *= hardware.queue_family_properties.queueCount;
	score *= hardware.queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	return score;
}

std::vector<const char*> get_instance_extensions()
{
	std::vector<const char*> extensions
		= sf::Vulkan::getGraphicsRequiredInstanceExtensions();
	extensions.push_back("VK_EXT_debug_utils");
	return extensions;
}

VulkanDevice create_device()
{
	VulkanDevice device = vkinit::device({
		.instance_layers     = {"VK_LAYER_KHRONOS_validation"},
		.instance_extensions = get_instance_extensions(),
		.device_layers       = {},
		.device_extensions   = {"VK_KHR_swapchain"},
		.score_device        = &score_device_hardware,
		.queue_priorities    = {1.0f},
	});
	return device;
}

VulkanWindow create_window(VulkanDevice device, int width, int height)
{
	VulkanWindow window = vkinit::window({
		.title        = "Torus",
		.shape        = {width, height},
		.depth_format = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.color_format = VK_FORMAT_B8G8R8A8_UNORM,
		.color_space  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR,
		.frame_count  = 3
	}, device);
	return window;
}

VulkanTarget create_target(VulkanDevice device, VulkanWindow window)
{
	VulkanTarget target = vkinit::target({
		.path_vertex_shader      = "assets/shaders/triangle.vert.spv",
		.path_fragment_shader    = "assets/shaders/fluid.frag.spv",
		.push_constants          = {},
		.descriptor_bindings     = {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr}
		},
		.vertex_input_bindings   = {
			{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX},
			{1, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_INSTANCE},
		},
		.vertex_input_attributes = {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
			{1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
			{2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
			{3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)},
			{4, 1, VK_FORMAT_R32G32B32_SFLOAT, 0},
		},
		.vertex_input_topology   = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	}, device, window);
	return target;
}
