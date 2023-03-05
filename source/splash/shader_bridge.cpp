#include "source/splash/shader_bridge.h"
#include "engine/vulkan/utility.h"
#include "engine/utility/alignment.h"

namespace kodanuki
{

ShaderBridge::ShaderBridge(ShaderBridgeBuilder builder)
: count_frame(builder.frame_count)
, count_index(builder.model.indices.size())
, count_vertex(builder.model.vertices.size())
, render_pipeline_layout(builder.render_pipeline.get_pipeline_layout())
, render_descriptor_layout(builder.render_pipeline.get_descriptor_layout())
, descriptor_pool(create_descriptor_pool(builder.device))
, device(builder.device)
, tensors(create_render_tensors(builder))
, simulation(builder.device, count_frame)
, cache({})
{
	simulation.load_scene(builder.scene);
	create_render_descriptors();
}

ShaderBridge::~ShaderBridge()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
}

void ShaderBridge::tick_simulation(uint32_t frame, float delta_time)
{
	simulation.tick_fluids(frame, delta_time);
}

uint32_t ShaderBridge::get_index_count()
{
	return count_index;
}

uint32_t ShaderBridge::get_instance_count()
{
	return simulation.get_particle_count();
}

void ShaderBridge::bind_render_resources(VkCommandBuffer buffer, uint32_t frame)
{
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline_layout, 0, 1, &render_descriptors[frame], 0, nullptr);
	VulkanTensor position = simulation.get_position(frame);

	std::vector<VkBuffer> buffers = {
		tensors.tensor_vertex.get_buffer(),
		position.get_buffer(),
	};
	std::vector<VkDeviceSize> offsets = {0, 0};

	vkCmdBindVertexBuffers(buffer, 0, 2, buffers.data(), offsets.data());
	vkCmdBindIndexBuffer(buffer, tensors.tensor_index.get_buffer(), 0, VK_INDEX_TYPE_UINT32);
}

void ShaderBridge::update_descriptor(VkDescriptorSet descriptor, uint32_t binding, VkDescriptorType type, VulkanTensor tensor, uint32_t frame)
{
	uint32_t size = tensor.get_byte_size(1);

	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = tensor.get_buffer();
	buffer_info.offset = frame * size;
	buffer_info.range = size;

	VkWriteDescriptorSet descriptor_write = {};
	descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.pNext = nullptr;
	descriptor_write.dstSet = descriptor;
	descriptor_write.dstBinding = binding;
	descriptor_write.dstArrayElement = 0;
	descriptor_write.descriptorCount = 1;
	descriptor_write.descriptorType = type;
	descriptor_write.pImageInfo = nullptr;
	descriptor_write.pBufferInfo = &buffer_info;
	descriptor_write.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(device, 1, &descriptor_write, 0, nullptr);
}

void ShaderBridge::create_render_descriptors()
{
	render_descriptors = create_descriptor_sets(device, descriptor_pool, render_descriptor_layout, count_frame);
	
	for (uint32_t i = 0; i < count_frame; i++) {
		update_descriptor(render_descriptors[i], 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			tensors.tensor_mvp, 0);
	}
}

RenderTensors ShaderBridge::create_render_tensors(ShaderBridgeBuilder builder)
{
	RenderTensors tensors = {
		.tensor_index = {{
			.device = device,
			.cache = cache,
			.shape = {1, align_modulo(count_index, 4)},
			.dtype = vt::eInt32,
			.dshare = vt::eUnique
		}},
		.tensor_vertex = {{
			.device = device,
			.cache = cache,
			.shape = {1, align_modulo(count_vertex * sizeof(Vertex), 16)},
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

}
