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
	}, device, window).expect("[Error] Failed to create target!");

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
	}).expect("[Error] Failed to create device!");

	VulkanWindow window = vkinit::window({
		.title        = "Torus",
		.shape        = {1950, 1200},
		.depth_format = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.color_format = VK_FORMAT_B8G8R8A8_UNORM,
		.color_space  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR,
		.frame_count  = 3
	}, device).expect("[Error] Failed to create window!");


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









// Dead Code, but still there so I can copy/pasta stuff ...

// struct tensor_t
// {
// 	vktype::buffer_t primary_buffer;
// 	vktype::buffer_t staging_buffer;
// 	vktype::memory_t primary_memory;
// 	vktype::memory_t staging_memory;
// 	std::vector<std::size_t> shape;
// 	uint32_t element_size;
// 	uint32_t element_count;
// };

// void record_command(vktype::command_buffer_t buffer, std::function<void()> callback)
// {
// 	VkCommandBufferBeginInfo buffer_info = {
// 		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
// 		.pNext = nullptr,
// 		.flags = 0,
// 		.pInheritanceInfo = nullptr
// 	};
// 	CHECK_VULKAN(vkResetCommandBuffer(buffer, 0));
// 	CHECK_VULKAN(vkBeginCommandBuffer(buffer, &buffer_info));
// 	callback();
// 	CHECK_VULKAN(vkEndCommandBuffer(buffer));
// }

// /**
//  * TODO
//  */
// vktype::tensor_t tensor(
// 	vktype::device_t         device,
// 	vktype::gpu_specs_t      gpu_specs,
// 	std::vector<std::size_t> shape,
// 	uint32_t                 element_size)
// {
// 	vktype::tensor_t tensor;

// 	std::size_t element_count = 1;
// 	for (auto dimension : shape) {
// 		element_count *= dimension;
// 	}
// 	tensor.shape = shape;
// 	tensor.element_size = element_size;
// 	tensor.element_count = element_count;

// 	VkBufferUsageFlags primary_usage_flags = 0;
// 	primary_usage_flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
// 	primary_usage_flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT; 
// 	primary_usage_flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
// 	primary_usage_flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
// 	primary_usage_flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
// 	primary_usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
// 	tensor.primary_buffer = vkinit::buffer(
// 		device,
// 		primary_usage_flags,
// 		element_count,
// 		element_size);

// 	VkBufferUsageFlags staging_usage_flags = 0;
// 	staging_usage_flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
// 	staging_usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
// 	tensor.staging_buffer = vkinit::buffer(
// 		device,
// 		staging_usage_flags,
// 		element_count,
// 		element_size);

// 	VkMemoryPropertyFlags primary_property_flags = 0;
// 	primary_property_flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
// 	VkMemoryRequirements primary_memory_requirements;
// 	vkGetBufferMemoryRequirements(device, tensor.primary_buffer, &primary_memory_requirements);
// 	tensor.primary_memory = vkinit::memory(
// 		device,
// 		gpu_specs,
// 		primary_memory_requirements,
// 		primary_property_flags);

// 	VkMemoryPropertyFlags staging_property_flags = 0;
// 	staging_property_flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
// 	staging_property_flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
// 	VkMemoryRequirements staging_memory_requirements;
// 	vkGetBufferMemoryRequirements(device, tensor.staging_buffer, &staging_memory_requirements);
// 	tensor.staging_memory = vkinit::memory(
// 		device,
// 		gpu_specs,
// 		staging_memory_requirements,
// 		staging_property_flags);

// 	vkBindBufferMemory(device, tensor.primary_buffer, tensor.primary_memory, 0);
// 	vkBindBufferMemory(device, tensor.staging_buffer, tensor.staging_memory, 0);
// 	return tensor;
// }

// void load_asnyc(VkCommandBuffer buffer, vktype::tensor_t tensor)
// {
// 	VkBufferCopy config = {
// 		.srcOffset = 0,
// 		.dstOffset = 0,
// 		.size = tensor.element_size * tensor.element_count
// 	};
// 	vkCmdCopyBuffer(buffer, tensor.primary_buffer, tensor.staging_buffer, 1, &config);
// }

// void save_asnyc(VkCommandBuffer buffer, vktype::tensor_t tensor)
// {
// 	VkBufferCopy config = {
// 		.srcOffset = 0,
// 		.dstOffset = 0,
// 		.size = tensor.element_size * tensor.element_count
// 	};
// 	vkCmdCopyBuffer(buffer, tensor.staging_buffer, tensor.primary_buffer, 1, &config);
// }



// struct MVP
// {
//     glm::mat4 model;
//     glm::mat4 view;
//     glm::mat4 projection;
// };

// template <typename T>
// T* tensor_mapping(vktype::device_t device, vktype::tensor_t tensor)
// {
// 	T* data;
// 	CHECK_VULKAN(vkMapMemory(device, tensor.staging_memory,
// 		0, tensor.element_size * tensor.element_count,
// 		0, (void**) &data));
// 	return data;
// }

// 	// vktype::command_buffer_t staging_command_buffer = vkinit2::command_buffer(
// 	// 	device2, target.command_pool);

// 	// vktype::tensor_t vertex_tensor = vkinit2::tensor(
// 	// 	device2, gpu_specs, {3}, sizeof(Vertex));

// 	// Vertex* vertex_data = tensor_mapping<Vertex>(device2, vertex_tensor);
// 	// vertex_data[0].position = {0, 0, 0};
// 	// vertex_data[1].position = {0, 1, 0};
// 	// vertex_data[2].position = {0, 0, 1};

// 	// vktype::tensor_t instance_tensor = vkinit2::tensor(
// 	// 	device2, gpu_specs, {1}, sizeof(glm::vec3));

// 	// glm::vec3* instance_data = tensor_mapping<glm::vec3>(device2, instance_tensor);
// 	// instance_data[0] = {0, 0, 0};

// 	// vktype::tensor_t index_tensor = vkinit2::tensor(
// 	// 	device2, gpu_specs, {3}, sizeof(uint32_t));

// 	// uint32_t* index_data = tensor_mapping<uint32_t>(device2, index_tensor);
// 	// index_data[0] = 0;
// 	// index_data[1] = 1;
// 	// index_data[2] = 2;

// 	// vkutil::record_command(staging_command_buffer, [&](){
// 	// 	vkmath::save_asnyc(staging_command_buffer, vertex_tensor);
// 	// 	vkmath::save_asnyc(staging_command_buffer, instance_tensor);
// 	// 	vkmath::save_asnyc(staging_command_buffer, index_tensor);
// 	// });
// 	// VkCommandBuffer native_staging_command_buffer = staging_command_buffer;
// 	// VkSubmitInfo info = {};
// 	// info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
// 	// info.commandBufferCount = 1;
// 	// info.pCommandBuffers = &native_staging_command_buffer;
// 	// CHECK_VULKAN(vkQueueSubmit(graphics_queue, 1, &info, VK_NULL_HANDLE));
// 	// CHECK_VULKAN(vkDeviceWaitIdle(device2));
