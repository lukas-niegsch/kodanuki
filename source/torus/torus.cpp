#include "engine/display/vkinit.h"
using namespace kodanuki;

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
		.frame_count  = 4
	}, device).expect("[Error] Failed to create window!");


	bool running = true;
	sf::WindowBase& native_window = window.window;

	while (running) {
		sf::Event event;
		while (native_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				running = false;
			} else if (event.type == sf::Event::Resized) {
				// TODO: recreate_renderer();
			}
		}
	
		// TODO: render
		// vkutil::aquire_frame(device, window);
		// ...
		// specify drawing commands
		// ...
		// vkutil::submit_frame(device, window, commands);
		// vkutil::render_frame(device, window);
	}
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

// namespace vkdraw
// {

// /**
//  * The timeout for drawing commands in (almost) nanoseconds.
//  */
// static uint64_t TIMEOUT = 1 /* seconds */ * 100000000;

// /**
//  * Aquires the current frame for rendering.
//  *
//  * @param device The device that renders the frame.
//  * @param target The target on which to render.
//  * @return Were there no swapchain errors?
//  */
// bool aquire_frame(
// 	vktype::device_t  device,
// 	vktype::target_t& target)
// {
// 	vktype::frame_t& frame = target.frames[target.submit_frame];
// 	VkFence aquire_fence = frame.aquire_frame_fence;
// 	CHECK_VULKAN(vkWaitForFences(device, 1, &aquire_fence, VK_TRUE, TIMEOUT));

// 	auto result = vkAcquireNextImageKHR(device, target.swapchain, TIMEOUT,
// 		frame.image_available_semaphore, VK_NULL_HANDLE, &target.render_frame);

// 	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
// 		return false;
// 	}

// 	if (result != VK_SUCCESS) {
// 		throw std::runtime_error("failed to acquire swap chain image!");
// 	}

// 	CHECK_VULKAN(vkResetFences(device, 1, &aquire_fence));
// 	return true;
// }

// /**
//  * Submits the draw commands to the GPU. This will start one renderpass
//  * and binds one pipeline, and then calls all the command before submitting.
//  *
//  * @param target The target on which to render.
//  * @param renderpass The renderpass for which to render.
//  * @param pipeline The pipeline for the single subpass.
//  * @param surface_extent The render area (typically the window extent).
//  * @param submit_queue The queue on which to submit the commands.
//  * @param commands The actual draw commands.
//  */
// void submit_frame(
// 	vktype::target_t&                                 target,
// 	vktype::renderpass_t                              renderpass,
// 	vktype::pipeline_t                                pipeline,
// 	VkExtent2D                                        surface_extent,
// 	VkQueue                                           submit_queue,
// 	std::vector<std::function<void(VkCommandBuffer)>> commands)
// {
// 	vktype::frame_t& frame = target.frames[target.submit_frame];
// 	VkCommandBuffer buffer = frame.command_buffer;

// 	std::array<VkClearValue, 2> clear_values = {};
// 	clear_values[0].color = {0.53f, 0.81f, 0.92f};
// 	clear_values[1].depthStencil = {1.0f, 0};

// 	vkutil::record_command(frame.command_buffer, [&]() {
// 		VkRenderPassBeginInfo renderpass_info = {};
// 		renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
// 		renderpass_info.renderPass = renderpass;
// 		renderpass_info.framebuffer = target.frames[target.render_frame].framebuffer;
// 		renderpass_info.renderArea.offset = {0, 0};
// 		renderpass_info.renderArea.extent = surface_extent;
// 		renderpass_info.clearValueCount = clear_values.size();
// 		renderpass_info.pClearValues = clear_values.data();
// 		vkCmdBeginRenderPass(buffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);
// 		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

// 		for (auto command_callback : commands) {
// 			command_callback(buffer);
// 		}
		
// 		vkCmdEndRenderPass(buffer);
// 	});

// 	VkSemaphore image_available = frame.image_available_semaphore;
// 	VkSemaphore render_finished = frame.render_finished_semaphore;
// 	VkFence aquire_frame = frame.aquire_frame_fence;
// 	VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

// 	VkSubmitInfo info = {};
// 	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
// 	info.waitSemaphoreCount = 1;
// 	info.pWaitSemaphores = &image_available;
// 	info.pWaitDstStageMask = &flags;
// 	info.commandBufferCount = 1;
// 	info.pCommandBuffers = &buffer;
// 	info.signalSemaphoreCount = 1;
// 	info.pSignalSemaphores = &render_finished;
// 	CHECK_VULKAN(vkQueueSubmit(submit_queue, 1, &info, aquire_frame));
// }

// /**
//  * Presents the current frame for rendering.
//  *
//  * @param target The target on which to render.
//  * @param submit_queue The queue on which to render.
//  * @return Were there no swapchain errors?
//  */
// bool render_frame(
// 	vktype::target_t& target,
// 	VkQueue           render_queue)
// {
// 	vktype::frame_t& frame = target.frames[target.submit_frame];
// 	VkSwapchainKHR swapchain = target.swapchain;
// 	VkSemaphore render_finished = frame.render_finished_semaphore;
	
// 	VkPresentInfoKHR info = {};
// 	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
// 	info.waitSemaphoreCount = 1;
// 	info.pWaitSemaphores = &render_finished;
// 	info.swapchainCount = 1;
// 	info.pSwapchains = &swapchain;
// 	info.pImageIndices = &target.render_frame;
// 	auto result = vkQueuePresentKHR(render_queue, &info);
// 	target.submit_frame = (target.submit_frame + 1) % target.max_frame;

// 	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
// 		return false;
// 	}

// 	if (result != VK_SUCCESS) {
// 		throw std::runtime_error("failed to present swap chain image!");
// 	}

// 	return true;
// }

// namespace vkmath
// {

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

// struct Vertex
// {
// 	glm::vec3 position;
// 	glm::vec3 color;
// 	glm::vec3 normal;
// 	glm::vec2 uv;

// 	bool operator==(const Vertex& other) const;
// };

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

// 	// vktype::renderpass_t renderpass = vkinit2::renderpass(
// 	// 	device2, img_specs);

// 	// vktype::descriptor_layout_t UBO_descriptor_layout = vkinit2::descriptor_layout(
// 	// 	device2, {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
// 	// 	VK_SHADER_STAGE_VERTEX_BIT, nullptr}});

// 	// vktype::pipeline_layout_t pipeline_layout = vkinit2::pipeline_layout(
// 	// 	device2, {UBO_descriptor_layout});

// 	// vktype::shader_t vertex_shader = vkinit2::shader(
// 	// 	device2, "assets/shaders/fluid.vert.spv");
	
// 	// vktype::shader_t fragment_shader = vkinit2::shader(
// 	// 	device2, "assets/shaders/fluid.frag.spv");

// 	// std::vector<VkVertexInputBindingDescription> input_bindings = {
// 	// 	{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX},
// 	// 	{1, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_INSTANCE},
// 	// };

// 	// std::vector<VkVertexInputAttributeDescription> input_attributes = {
// 	// 	{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
// 	// 	{1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
// 	// 	{2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
// 	// 	{3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)},
// 	// 	{4, 1, VK_FORMAT_R32G32B32_SFLOAT, 0},
// 	// };

// 	// vktype::descriptor_pool_t descriptor_pool = vkinit2::descriptor_pool(
// 	// 	device2, 1, 1);

// 	// vktype::descriptor_set_t MPV_descriptor = vkinit2::descriptor_set(
// 	// 	device2, descriptor_pool, UBO_descriptor_layout);

// 	// vktype::surface_t surface;
// 	// vktype::target_t target;
// 	// vktype::pipeline_t pipeline;
	
// 	// auto recreate_renderer = [&]() {
// 	// 	CHECK_VULKAN(vkDeviceWaitIdle(device2));

// 	// 	vktype::surface_t new_surface = vkinit2::surface(
// 	// 		instance, [&](VkInstance instance, VkSurfaceKHR& surface) {
// 	// 			window.createVulkanSurface(instance, surface); });

// 	// 	target = vkinit2::target(
// 	// 		device2, renderpass, gpu_specs, img_specs, new_surface);
		
// 	// 	// prevent deletion of surface before target's swapchain
// 	// 	surface = new_surface;

// 	// 	pipeline = vkinit2::graphics_pipeline(
// 	// 		device2, renderpass, pipeline_layout, vertex_shader,
// 	// 		fragment_shader, vkutil::get_surface_extent(gpu_specs, surface),
// 	// 		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, input_bindings,
// 	// 		input_attributes);
// 	// };
// 	// recreate_renderer();

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


// 	// 	if (!vkdraw::aquire_frame(device2, target)) {
// 	// 		recreate_renderer();
// 	// 		continue;
// 	// 	}

// 	// 	std::vector<std::function<void(VkCommandBuffer)>> commands;
// 	// 	commands.push_back([=](VkCommandBuffer buffer) {
// 	// 		(void) buffer;
// 	// 	});

// 	// 	vkdraw::submit_frame(target, renderpass, pipeline,
// 	// 		vkutil::get_surface_extent(gpu_specs, surface), graphics_queue,
// 	// 		commands);

// 	// 	if (!vkdraw::render_frame(target, graphics_queue)) {
// 	// 		recreate_renderer();
// 	// 	}
// 	// }

// 	// CHECK_VULKAN(vkDeviceWaitIdle(device2));
// 	// return 0;
// }