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
, size_index(align_modulo(sizeof(uint32_t), 16))
, size_vertex(align_modulo(sizeof(Vertex), 16))
, size_mass(align_modulo(sizeof(float), 16))
, size_pressure(align_modulo(sizeof(float), 16))
, size_density(align_modulo(sizeof(float), 16))
, size_mvp(align<MVP, 256>::size)
, size_ud(align<UD, 256>::size)
, size_position(align<glm::vec3, 16>::size)
, size_velocity(align<glm::vec3, 16>::size)
, offset_indices(0)
, offset_vertices(offset_indices + count_index * size_index)
, offset_mass(offset_vertices + count_vertex * size_vertex)
, offset_pressure(offset_mass + count_instance * size_mass)
, offset_density(offset_pressure + count_instance * size_pressure)
, offset_total(align_modulo(offset_density + count_instance * size_density, 256))
, offset_frame_mvp(0)
, offset_frame_ud(offset_frame_mvp + 1 * size_mvp)
, offset_frame_position(offset_frame_ud + 1 * size_ud)
, offset_frame_velocity(offset_frame_position + count_instance * size_position)
, offset_frame_total(align_modulo(offset_frame_velocity + count_instance * size_velocity, 256))
, render_pipeline_layout(builder.render_pipeline.get_pipeline_layout())
, update_pressure_pipeline_layout(builder.update_pipeline_pressure.get_pipeline_layout())
, update_simulate_pipeline_layout(builder.update_pipeline_simulate.get_pipeline_layout())
, render_descriptor_layout(builder.render_pipeline.get_descriptor_layout())
, update_pressure_descriptor_layout(builder.update_pipeline_pressure.get_descriptor_layout())
, update_simulate_descriptor_layout(builder.update_pipeline_simulate.get_descriptor_layout())
, descriptor_pool(create_descriptor_pool(builder.device))
, device(builder.device)
, memory(allocate_memory(builder.model, builder.scene))
, memory_buffer(memory)
, cache({})
, tensor_index({device, cache, {1, count_index}, vt::eInt32, vt::eUnique})
, tensor_vertex({device, cache, {1, count_vertex * size_vertex}, vt::eByte, vt::eUnique})
, tensor_mass({device, cache, {1, count_instance}, vt::eFloat, vt::eUnique})
, tensor_mvp({device, cache, {count_frame, size_mvp}, vt::eByte, vt::eShared})
, tensor_ud({device, cache, {count_frame, size_ud}, vt::eByte, vt::eShared})
, tensor_position({device, cache, {count_frame, 4 * count_instance}, vt::eFloat, vt::eUnique})
, tensor_velocity({device, cache, {count_frame, 4 * count_instance}, vt::eFloat, vt::eUnique})
, tensor_pressure({device, cache, {count_frame, count_instance}, vt::eFloat, vt::eUnique})
, tensor_density({device, cache, {count_frame, count_instance}, vt::eFloat, vt::eUnique})
{
	load_tensor_data(builder.model, builder.scene);
	print_memory_layout();
	create_render_descriptors();
	create_update_pressure_descriptors();
	create_update_simulate_descriptors();
}

ShaderBridge::~ShaderBridge()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
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

VulkanMemory::StagingBuffer<align<MVP, 256>> ShaderBridge::get_buffer_mvp(uint32_t frame)
{
	return memory.create_buffer<align<MVP, 256>>({
		.offset = offset_total + frame * offset_frame_total + offset_frame_mvp,
		.count = 1
	});
}

VulkanMemory::StagingBuffer<align<UD, 256>> ShaderBridge::get_buffer_ud(uint32_t frame)
{
	return memory.create_buffer<align<UD, 256>>({
		.offset = offset_total + frame * offset_frame_total + offset_frame_ud,
		.count = 1
	});
}

void ShaderBridge::print_memory_layout()
{
	return;
	std::cout << "count_frame: " << count_frame << '\n';
	std::cout << "count_index: " << count_index << '\n';
	std::cout << "count_vertex: " << count_vertex << '\n';
	std::cout << "count_instance: " << count_instance << '\n';
	std::cout << "size_index: " << size_index << '\n';
	std::cout << "size_vertex: " << size_vertex << '\n';
	std::cout << "size_mass: " << size_mass << '\n';
	std::cout << "size_density: " << size_density << '\n';
	std::cout << "size_pressure: " << size_pressure << '\n'; 
	std::cout << "size_mvp: " << size_mvp << '\n';
	std::cout << "size_ud: " << size_ud << '\n';
	std::cout << "size_position: " << size_position << '\n';
	std::cout << "size_velocity: " << size_velocity << '\n';
	std::cout << "offset_indices: " << offset_indices << '\n';
	std::cout << "offset_vertices: " << offset_vertices << '\n';
	std::cout << "offset_mass: " << offset_mass << '\n';
	std::cout << "offset_density: " << offset_density << '\n';
	std::cout << "offset_pressure: " << offset_pressure << '\n';	
	std::cout << "offset_total: " << offset_total << '\n';
	std::cout << "offset_frame_mvp: " << offset_frame_mvp << '\n';
	std::cout << "offset_frame_ud: " << offset_frame_ud << '\n';
	std::cout << "offset_frame_position: " << offset_frame_position << '\n';
	std::cout << "offset_frame_velocity: " << offset_frame_velocity << '\n';
	std::cout << "offset_frame_total: " << offset_frame_total << '\n';
	std::cout << "total_memory_bytes: " << offset_total + count_frame * offset_frame_total << '\n';

	std::size_t new_total = 0;
	new_total += tensor_index.get_byte_size();
	new_total += tensor_vertex.get_byte_size();
	new_total += tensor_mass.get_byte_size();
	new_total += tensor_mvp.get_byte_size();
	new_total += tensor_ud.get_byte_size();
	new_total += tensor_position.get_byte_size();
	new_total += tensor_velocity.get_byte_size();
	new_total += tensor_pressure.get_byte_size();
	new_total += tensor_density.get_byte_size();
	std::cout << "new_total: " << new_total << '\n';
	std::cout << "size tensor_index: " << tensor_index.get_byte_size() << '\n';
	std::cout << "size tensor_vertex: " << tensor_vertex.get_byte_size() << '\n';
	std::cout << "size tensor_mass: " << tensor_mass.get_byte_size() << '\n';
	std::cout << "size tensor_mvp: " << tensor_mvp.get_byte_size() << '\n';
	std::cout << "size tensor_ud: " << tensor_ud.get_byte_size() << '\n';
	std::cout << "size tensor_position: " << tensor_position.get_byte_size() << '\n';
	std::cout << "size tensor_velocity: " << tensor_velocity.get_byte_size() << '\n';
	std::cout << "size tensor_pressure: " << tensor_pressure.get_byte_size() << '\n';
	std::cout << "size tensor_density: " << tensor_density.get_byte_size() << '\n';
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
	VkDeviceSize offset_vertex = offset_vertices;
	vkCmdBindVertexBuffers(buffer, 0, 1, &memory_buffer, &offset_vertex);
	VkDeviceSize offset_instance = offset_total + frame * offset_frame_total + offset_frame_position;
	vkCmdBindVertexBuffers(buffer, 1, 1, &memory_buffer, &offset_instance);
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

VulkanMemory ShaderBridge::allocate_memory(Model model, Scene scene)
{
	assert(sizeof(MVP) <= 256); // Vulkan requires uniform buffers to be 256 aligned!

	VulkanMemory memory = {{
		.device = device,
		.byte_size = offset_total + count_frame * offset_frame_total
	}};

	// Setting the memory for the vertices:
	{
		auto buffer = memory.create_buffer<Vertex>({.offset = offset_vertices, .count = count_vertex});
		for (uint32_t i = 0; i < count_vertex; i++) {
			buffer[i] = model.vertices[i];
			buffer[i].color = {0.0f, 0.0f, 0.8f};
		}
		buffer.slow_push();
	}

	// Setting the memory for the masses:
	{
		auto buffer = memory.create_buffer<float>({.offset = offset_mass, .count = count_instance});
		for (uint32_t i = 0; i < count_instance; i++) {
			buffer[i] = scene.masses[i];
		}
		buffer.slow_push();
	}

	// Density will be handled by the shader!
	// Pressure will be handled by the shader!

	for (uint32_t frame = 0; frame < count_frame; frame++) {
		uint32_t offset_frame = offset_total + frame * offset_frame_total;
		// MPV will be handled by the user!
		// UD will be handled by the user!

		// Setting the memory for the positions:
		{
			auto buffer = memory.create_buffer<align<glm::vec3, 16>>({.offset = offset_frame + offset_frame_position , .count = count_instance});
			for (uint32_t i = 0; i < count_instance; i++) {
				buffer[i] = scene.positions[i];
			}
			buffer.slow_push();
		}

		// Setting the memory for the velocities:
		{
			auto buffer = memory.create_buffer<align<glm::vec3, 16>>({.offset = offset_frame + offset_frame_velocity, .count = count_instance});
			for (uint32_t i = 0; i < count_instance; i++) {
				buffer[i] = scene.velocities[i];
			}
			buffer.slow_push();
		}
	}

	return memory;
}


void ShaderBridge::update_descriptor(VkDescriptorSet descriptor, uint32_t binding, VkDescriptorType type, VkDeviceSize offset, VkDeviceSize size)
{
	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = memory;
	buffer_info.offset = offset;
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
			offset_total + i * offset_frame_total + offset_frame_mvp, sizeof(MVP));
	}
}

void ShaderBridge::create_update_pressure_descriptors()
{
	update_pressure_descriptors = create_descriptor_sets(device, descriptor_pool, update_pressure_descriptor_layout, count_frame);

	for (uint32_t i = 0; i < count_frame; i++) {
		update_descriptor(update_pressure_descriptors[i], 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			offset_total + i * offset_frame_total + offset_frame_ud, sizeof(UD));

		update_descriptor(update_pressure_descriptors[i], 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_total + ((i - 1) % count_frame) * offset_frame_total + offset_frame_position, size_position * count_instance);

		update_descriptor(update_pressure_descriptors[i], 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_mass, size_mass * count_instance);

		update_descriptor(update_pressure_descriptors[i], 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_density, size_density * count_instance);

		update_descriptor(update_pressure_descriptors[i], 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_pressure, size_pressure * count_instance);
	}
}

void ShaderBridge::create_update_simulate_descriptors()
{
	update_simulate_descriptors = create_descriptor_sets(device, descriptor_pool, update_simulate_descriptor_layout, count_frame);

	for (uint32_t i = 0; i < count_frame; i++) {
		update_descriptor(update_simulate_descriptors[i], 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			offset_total + i * offset_frame_total + offset_frame_ud, sizeof(UD));

		update_descriptor(update_simulate_descriptors[i], 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_total + ((i - 1) % count_frame) * offset_frame_total + offset_frame_position, size_position * count_instance);

		update_descriptor(update_simulate_descriptors[i], 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_total + ((i - 1) % count_frame) * offset_frame_total + offset_frame_velocity, size_velocity * count_instance);

		update_descriptor(update_simulate_descriptors[i], 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_mass, size_mass * count_instance);

		update_descriptor(update_simulate_descriptors[i], 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_density, size_density * count_instance);

		update_descriptor(update_simulate_descriptors[i], 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_pressure, size_pressure * count_instance);

		update_descriptor(update_simulate_descriptors[i], 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_total + i * offset_frame_total + offset_frame_position, size_position * count_instance);

		update_descriptor(update_simulate_descriptors[i], 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			offset_total + i * offset_frame_total + offset_frame_velocity, size_velocity * count_instance);
	}
}




void ShaderBridge::load_tensor_data(Model model, Scene scene)
{
	// Setting the memory for the indices:
	tensor_index.with_maps<uint32_t>([&](std::vector<uint32_t>& values) {
		for (uint32_t i = 0; i < count_index; i++) {
			values[i] = model.indices[i];
		}
	});

	// Setting the memory for the vertices:
	tensor_vertex.with_maps<Vertex>([&](std::vector<Vertex>& values) {
		for (uint32_t i = 0; i < count_vertex; i++) {
			values[i] = model.vertices[i];
			values[i].color = {0.0f, 0.0f, 0.8f};
		}
	});

	(void) model; (void) scene;
}

}
