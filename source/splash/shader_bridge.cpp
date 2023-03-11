#include "source/splash/shader_bridge.h"
#include "engine/utility/alignment.h"

namespace kodanuki
{

ShaderBridge::ShaderBridge(ShaderBridgeBuilder builder)
: count_frame(builder.frame_count)
, count_index(builder.model.indices.size())
, count_vertex(builder.model.vertices.size())
, render_pipeline_layout(builder.render_pipeline.get_pipeline_layout())
, render_descriptor_layout(builder.render_pipeline.get_descriptor_layout())
, descriptor_pool(builder.device.get_descriptor_pool())
, device(builder.device)
, tensors(create_render_tensors(builder))
, cache({})
{
	create_render_descriptors();
}

uint32_t ShaderBridge::get_index_count()
{
	return count_index;
}

void ShaderBridge::bind_render_resources(VkCommandBuffer buffer, uint32_t frame, VulkanTensor positions)
{
	VkDescriptorSet descriptor = render_descriptors[frame];
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline_layout, 0, 1, &descriptor, 0, nullptr);

	std::vector<VkBuffer> buffers = {
		tensors.tensor_vertex.get_buffer(),
		positions.get_buffer(),
	};
	std::vector<VkDeviceSize> offsets = {0, 0};

	vkCmdBindVertexBuffers(buffer, 0, 2, buffers.data(), offsets.data());
	vkCmdBindIndexBuffer(buffer, tensors.tensor_index.get_buffer(), 0, VK_INDEX_TYPE_UINT32);
}

void ShaderBridge::create_render_descriptors()
{
	for (uint32_t i = 0; i < count_frame; i++) {
		render_descriptors.push_back(create_descriptor_set(device, descriptor_pool, render_descriptor_layout));
		tensors.tensor_mvp.update_descriptor(render_descriptors[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
	}
}

RenderTensors ShaderBridge::create_render_tensors(ShaderBridgeBuilder builder)
{
	RenderTensors tensors = {
		.tensor_index = {{
			.device = device,
			.cache = cache,
			.shape = {1, count_index},
			.dtype = vt::eInt32,
			.dshare = vt::eUnique
		}},
		.tensor_vertex = {{
			.device = device,
			.cache = cache,
			.shape = {1, count_vertex * sizeof(Vertex)},
			.dtype = vt::eByte,
			.dshare = vt::eUnique
		}},
		.tensor_mvp = {{
			.device = device,
			.cache = cache,
			.shape = {count_frame, align<MVP, 256>::size},
			.dtype = vt::eByte,
			.dshare = vt::eShared
		}}
	};
	load_tensor_data(builder.model);
	return tensors;
}

void ShaderBridge::load_tensor_data(Model model)
{
	tensors.tensor_index.load_data(model.indices);
	tensors.tensor_vertex.load_data(model.vertices);

	tensors.tensor_vertex.with_maps<Vertex>([](std::vector<Vertex>& values) {
		for (uint32_t i = 0; i < values.size(); i++) {
			values[i].color = {0.0f, 0.0f, 0.8f}; // color all spheres blue
		}
	});
}

void ShaderBridge::update_mvp(const MVP& new_mvp, uint32_t frame)
{
	tensors.tensor_mvp.with_maps<align<MVP, 256>>([&](std::vector<align<MVP, 256>>& values) {
		values[frame] = new_mvp;
	});
}

VulkanTensor ShaderBridge::get_mvp(uint32_t frame)
{
	(void) frame;
	return tensors.tensor_mvp;
}

}
