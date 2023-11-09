#include "engine/display/vkinit.h"
#include "engine/display/vkutil.h"
#include <glm/glm.hpp>
using namespace kodanuki;


struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;

	bool operator==(const Vertex& other) const;
};

std::vector<const char*> get_instance_extensions()
{
	std::vector<const char*> extensions
		= sf::Vulkan::getGraphicsRequiredInstanceExtensions();
	extensions.push_back("VK_EXT_debug_utils");
	return extensions;
}

int score_device_hardware(vktype::hardware_t hardware)
{
	int score = 0;
	score += hardware.physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	score *= hardware.queue_family_properties.queueCount;
	score *= hardware.queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	return score;
}

VulkanTarget create_torus_target(VulkanDevice device, VulkanWindow window)
{
	VulkanTarget target = vkinit::target({
		.path_vertex_shader      = "assets/shaders/fluid.vert.spv",
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
	}, device, window).expect("Failed to create target!");

	return target;
}

int main()
{
	VulkanDevice device = vkinit::device({
		.instance_layers     = {"VK_LAYER_KHRONOS_validation"},
		.instance_extensions = get_instance_extensions(),
		.device_layers       = {},
		.device_extensions   = {"VK_KHR_swapchain"},
		.score_device        = &score_device_hardware,
		.queue_priorities    = {1.0f},
	}).expect("Failed to create device!");

	VulkanWindow window = vkinit::window({
		.title        = "Torus",
		.shape        = {1950, 1200},
		.depth_format = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.color_format = VK_FORMAT_B8G8R8A8_UNORM,
		.color_space  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR,
		.frame_count  = 3
	}, device).expect("Failed to create window!");


	VulkanTensor vertex_tensor = vkinit::tensor({
		.shape        = {window.image_specs.frame_count, 3},
		.element_size = sizeof(Vertex),
		.usage        = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	}, device).expect("Failed to create vertex tensor!");

	VulkanTensor instance_tensor = vkinit::tensor({
		.shape        = {window.image_specs.frame_count, 1},
		.element_size = sizeof(glm::vec3),
		.usage        = 0,
	}, device).expect("Failed to create instance tensor!");

	VulkanTensor index_tensor = vkinit::tensor({
		.shape        = {window.image_specs.frame_count, 3},
		.element_size = sizeof(uint32_t),
		.usage        = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	}, device).expect("Failed to create index tensor!");

	VulkanTensor mvp_tensor = vkinit::tensor({
		.shape        = {window.image_specs.frame_count, 3},
		.element_size = sizeof(glm::mat4),
		.usage        = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	}, device).expect("Failed to create mpv tensor!");


	bool running = true;
	sf::WindowBase& native_window = window.window;
	VulkanTarget target = create_torus_target(device, window);

	while (running) {
		sf::Event event;
		while (native_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				running = false;
			} else if (event.type == sf::Event::Resized) {
				window.recreate(device);
				target = create_torus_target(device, window);
			}
		}

		vkutil::aquire_frame(device, window);
		vkutil::record_frame(device, window, {});
		vkutil::submit_frame(device, window);
		vkutil::render_frame(device, window);
	}

	CHECK_VULKAN(vkDeviceWaitIdle(device));
	return 0;
}
