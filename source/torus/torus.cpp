#include "source/torus/vkbridge.h"


struct RenderTensors
{
	VulkanTensor vertex_tensor;
	VulkanTensor instance_tensor;
	VulkanTensor index_tensor;
	VulkanTensor mvp_tensor;
};

RenderTensors get_render_tensors(VulkanDevice device, VulkanWindow window)
{
	VulkanTensor vertex_tensor = vkinit::tensor({
		.shape        = {window.image_specs.frame_count, 3},
		.element_size = sizeof(Vertex),
		.usage        = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	}, device);

	VulkanTensor instance_tensor = vkinit::tensor({
		.shape        = {window.image_specs.frame_count, 1},
		.element_size = sizeof(glm::vec3),
		.usage        = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	}, device);

	VulkanTensor index_tensor = vkinit::tensor({
		.shape        = {window.image_specs.frame_count, 3},
		.element_size = sizeof(uint32_t),
		.usage        = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	}, device);

	VulkanTensor mvp_tensor = vkinit::tensor({
		.shape        = {window.image_specs.frame_count, 3},
		.element_size = sizeof(glm::mat4),
		.usage        = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	}, device);

	
	for (uint32_t i = 0; i < window.image_specs.frame_count; i++) {
		vertex_tensor.get<Vertex>({i, 0}).position = { 0.0, -0.5, 0};
		vertex_tensor.get<Vertex>({i, 1}).position = { 0.5,  0.5, 0};
		vertex_tensor.get<Vertex>({i, 2}).position = {-0.5,  0.5, 0};
		vertex_tensor.get<Vertex>({i, 0}).color = {1, 0, 0};
		vertex_tensor.get<Vertex>({i, 1}).color = {0, 1, 0};
		vertex_tensor.get<Vertex>({i, 2}).color = {0, 0, 1};
		instance_tensor.get<glm::vec3>({i, 0}) = {0, 0, 0};
		index_tensor.get<uint32_t>({i, 0}) = 0;
		index_tensor.get<uint32_t>({i, 1}) = 1;
		index_tensor.get<uint32_t>({i, 2}) = 2;
		mvp_tensor.get<glm::mat4>({i, 0}) = glm::mat4(1.0f);
		mvp_tensor.get<glm::mat4>({i, 1}) = glm::mat4(1.0f);
		mvp_tensor.get<glm::mat4>({i, 2}) = glm::mat4(1.0f);
	}

	RenderTensors result;
	result.vertex_tensor = vertex_tensor;
	result.instance_tensor = instance_tensor;
	result.index_tensor = index_tensor;
	result.mvp_tensor = mvp_tensor;
	return result;
}

vkdraw::fn_draw draw_triangle(VulkanTarget target, RenderTensors tensors)
{
	auto fn = vkdraw::indexed({
		.index_count    = 3,
		.instance_count = 1,
		.pipeline       = target.graphics_pipeline,
		.indices        = tensors.index_tensor,
		.vertices       = {
			tensors.vertex_tensor,
			tensors.instance_tensor,
		},
	});
	return fn;
}

int main()
{
	VulkanDevice device = create_device();
	VulkanWindow window = create_window(device, 1950, 1200);
	VulkanTarget target = create_target(device, window);

	bool running = true;
	sf::WindowBase& native_window = window.window;
	RenderTensors tensors    = get_render_tensors(device, window);
	vkdraw::fn_draw triangle = draw_triangle(target, tensors);

	while (running) {
		sf::Event event;
		while (native_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				running = false;
			} else if (event.type == sf::Event::Resized) {
				window.recreate(device);
			}
		}

		vkdraw::aquire_frame(device, window);
		vkdraw::record_frame(device, window, {triangle});
		vkdraw::submit_frame(device, window);
		vkdraw::render_frame(device, window);
	}

	CHECK_VULKAN(vkDeviceWaitIdle(device));
	return 0;
}
