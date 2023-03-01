#include "source/splash/shader_bridge.h"
#include "engine/vulkan/utility.h"

namespace kodanuki
{

uint32_t align_modulo(uint32_t value, uint32_t mod)
{
	return (value / mod + (value % mod != 0)) * mod;
}

ShaderBridge::ShaderBridge(ShaderBridgeBuilder builder)
: count_frame(builder.frame_count)
, count_index(builder.model.indices.size())
, count_vertex(builder.model.vertices.size())
, count_instance(builder.scene.instance_count)
, render_pipeline_layout(builder.render_pipeline.get_pipeline_layout())
, update_pressure_pipeline_layout(builder.update_pipeline_pressure.get_pipeline_layout())
, update_simulate_pipeline_layout(builder.update_pipeline_simulate.get_pipeline_layout())
, render_descriptor_layout(builder.render_pipeline.get_descriptor_layout())
, update_pressure_descriptor_layout(builder.update_pipeline_pressure.get_descriptor_layout())
, update_simulate_descriptor_layout(builder.update_pipeline_simulate.get_descriptor_layout())
, descriptor_pool(create_descriptor_pool(builder.device))
, device(builder.device)
, cache({})
, tensor_index({device, cache, {1, align_modulo(count_index, 16)}, vt::eInt32, vt::eUnique})
, tensor_vertex({device, cache, {1, count_vertex * align_modulo(sizeof(Vertex), 16)}, vt::eByte, vt::eUnique})
, tensor_mass({device, cache, {1, align_modulo(count_instance, 16)}, vt::eFloat, vt::eUnique})
, tensor_mvp({device, cache, {count_frame, align<MVP, 256>::size}, vt::eByte, vt::eShared})
, tensor_ud({device, cache, {count_frame, align<UD, 256>::size}, vt::eByte, vt::eShared})
, tensor_position({device, cache, {count_frame, align_modulo(3 * count_instance, 16)}, vt::eFloat, vt::eUnique})
, tensor_velocity({device, cache, {count_frame, align_modulo(3 * count_instance, 16)}, vt::eFloat, vt::eUnique})
, tensor_pressure({device, cache, {count_frame, align_modulo(count_instance, 16)}, vt::eFloat, vt::eUnique})
, tensor_density({device, cache, {count_frame, align_modulo(count_instance, 16)}, vt::eFloat, vt::eUnique})
{
	load_tensor_data(builder.model, builder.scene);
	create_render_descriptors();
	create_update_pressure_descriptors();
	create_update_simulate_descriptors();
}

ShaderBridge::~ShaderBridge()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
}

uint32_t ShaderBridge::get_index_count()
{
	return count_index;
}

uint32_t ShaderBridge::get_instance_count()
{
	return count_instance;
}

void ShaderBridge::bind_render_resources(VkCommandBuffer buffer, uint32_t frame)
{
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline_layout, 0, 1, &render_descriptors[frame], 0, nullptr);
	
	std::vector<VkBuffer> buffers = {
		tensor_position.get_buffer(),
		tensor_vertex.get_buffer(),
	};

	std::vector<VkDeviceSize> offsets = {
		frame * (tensor_position.get_byte_size() / count_frame),
		0
	};

	vkCmdBindVertexBuffers(buffer, 0, 1, &buffers[0], &offsets[0]);
	vkCmdBindVertexBuffers(buffer, 1, 1, &buffers[1], &offsets[1]);
	VkBuffer buffer_index = tensor_index.get_buffer();
	vkCmdBindIndexBuffer(buffer, buffer_index, 0, VK_INDEX_TYPE_UINT32);
}

void ShaderBridge::bind_update_pressure_resources(VkCommandBuffer buffer, uint32_t frame)
{
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, update_pressure_pipeline_layout, 0, 1, &update_pressure_descriptors[frame], 0, nullptr);
}

void ShaderBridge::bind_update_simulate_resources(VkCommandBuffer buffer, uint32_t frame)
{
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, update_simulate_pipeline_layout, 0, 1, &update_simulate_descriptors[frame], 0, nullptr);
}

void ShaderBridge::update_descriptor(VkDescriptorSet descriptor, uint32_t binding, VkDescriptorType type, VulkanTensor tensor, uint32_t frame)
{
	uint32_t size = tensor.get_byte_size() / count_frame;

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
			tensor_mvp, 0);
	}
}

void ShaderBridge::create_update_pressure_descriptors()
{
	update_pressure_descriptors = create_descriptor_sets(device, descriptor_pool, update_pressure_descriptor_layout, count_frame);

	for (uint32_t i = 0; i < count_frame; i++) {
		update_descriptor(update_pressure_descriptors[i], 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			tensor_ud, 0);

		update_descriptor(update_pressure_descriptors[i], 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_position, (i - 1) % count_frame);

		update_descriptor(update_pressure_descriptors[i], 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_mass, 0);

		update_descriptor(update_pressure_descriptors[i], 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_density, i);

		update_descriptor(update_pressure_descriptors[i], 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_pressure, i);
	}
}

void ShaderBridge::create_update_simulate_descriptors()
{
	update_simulate_descriptors = create_descriptor_sets(device, descriptor_pool, update_simulate_descriptor_layout, count_frame);

	for (uint32_t i = 0; i < count_frame; i++) {
		update_descriptor(update_simulate_descriptors[i], 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			tensor_ud, 0);

		update_descriptor(update_simulate_descriptors[i], 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_position, (i - 1) % count_frame);

		update_descriptor(update_simulate_descriptors[i], 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_velocity, (i - 1) % count_frame);

		update_descriptor(update_simulate_descriptors[i], 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_mass, 0);

		update_descriptor(update_simulate_descriptors[i], 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_density, i);

		update_descriptor(update_simulate_descriptors[i], 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_pressure, i);

		update_descriptor(update_simulate_descriptors[i], 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_position, i);

		update_descriptor(update_simulate_descriptors[i], 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			tensor_velocity, i);
	}
}

void ShaderBridge::load_tensor_data(Model model, Scene scene)
{
	tensor_index.load_data(model.indices);
	tensor_vertex.load_data(model.vertices);
	tensor_mass.load_data(scene.masses);

	for (uint32_t frame = 0; frame < count_frame; frame++) {
		uint32_t offset = frame * count_instance;
		tensor_position.load_data(scene.positions, offset);
		tensor_velocity.load_data(scene.velocities, offset);
	}
}

void ShaderBridge::update_mvp(const MVP& new_mvp, uint32_t frame)
{
	tensor_mvp.with_maps<MVP>([&](std::vector<MVP>& values) {
		values[frame] = new_mvp;
	});
}

void ShaderBridge::update_ud(const UD& new_ud, uint32_t frame)
{
	tensor_mvp.with_maps<UD>([&](std::vector<UD>& values) {
		values[frame] = new_ud;
	});
}

}
