#ifdef INCLUDE_TENSOR_INLINE_HEADER
#include "engine/vulkan/debug.h"
#include <optional>
#include <iostream>
#include <cmath>
#include <cassert>

namespace kodanuki
{

uint32_t align_modulo(uint32_t value, uint32_t mod)
{
	return (value / mod + (value % mod != 0)) * mod;
}

struct TensorState
{
	VulkanDevice device;
	VulkanPipelineCache& cache;
	std::vector<std::size_t> shape;
	VulkanTensor::MemoryDataType dtype;
	VulkanTensor::MemorySharing dshare;
	std::optional<VkBuffer> primary_buffer;
	std::optional<VkBuffer> staging_buffer;
	std::optional<VkDeviceMemory> primary_memory;
	std::optional<VkDeviceMemory> staging_memory;
	VkCommandPool command_pool;
	VkCommandBuffer transfer_buffer;
	~TensorState();
};

template <typename T>
void VulkanTensor::with_maps(std::function<void(std::vector<T>&)> callback, uint32_t offset)
{
	with_mapped_memory<T>([&](T* data){
		if (state->dshare == eUnique) {
			VkBufferCopy config = {0, 0, get_byte_size()};
			copy_buffer(state->primary_buffer.value(), state->staging_buffer.value(), config);
		}

		std::vector<T> values(data, data + (get_byte_size() - offset) / sizeof(T));
		callback(values);
		std::copy(values.begin(), values.end(), data);

		if (state->dshare == eUnique) {
			VkBufferCopy config = {0, 0, get_byte_size()};
			copy_buffer(state->staging_buffer.value(), state->primary_buffer.value(), config);
		}
	}, offset);
}

template <typename T>
void VulkanTensor::with_mapped_memory(std::function<void(T*)> callback, uint32_t offset)
{
	VkDeviceMemory memory = [&](){
		switch (state->dshare) {
		case eUnique: return state->staging_memory.value();
		case eShared: return state->primary_memory.value();
		}
		throw std::runtime_error("MemorySharing unsupported!"); 
	}();

	void* data;
	CHECK_VULKAN(vkMapMemory(state->device, memory, offset, get_byte_size() - offset, 0, &data));
	T* typed_data = static_cast<T*>(data);
	callback(typed_data);
	vkUnmapMemory(state->device, memory);
}

template <typename T>
void VulkanTensor::slow_execute_linear(VulkanTensor tensorZ, T alpha, VulkanTensor tensorA, T beta, VulkanTensor tensorB)
{
	tensorA.with_maps<T>([&](std::vector<T>& valuesA) {
		tensorB.with_maps<T>([&](std::vector<T>& valuesB) {
			tensorZ.with_maps<T>([&](std::vector<T>& valuesC) {
				for (std::size_t i = 0; i < valuesA.size(); i++) {
					valuesC[i] = alpha * valuesA[i] + beta * valuesB[i];
				}
			});
		});
	});
}

template <typename T>
void VulkanTensor::slow_execute_fill(VulkanTensor tensor, T value)
{
	tensor.with_maps<T>([&](std::vector<T>& values){
		for (std::size_t i = 0; i < values.size(); i++) {
			values[i] = value;
		}
	});
}

template <typename T>
void VulkanTensor::load_data(const std::vector<T> values, uint32_t offset)
{
	with_maps<T>([&](std::vector<T>& buffer) {
		for (uint32_t i = 0; i < buffer.size(); i++) {
			buffer[i] = values[i];
		}
	}, offset);
}

template <typename T>
void VulkanTensor::fill(VulkanTensor& tensor, const T& value)
{
	slow_execute_fill(tensor, value);
}

}

#else // INCLUDE_TENSOR_INLINE_HEADER
#include "engine/vulkan/tensor.h"
#include "engine/vulkan/utility.h"

namespace kodanuki
{

TensorState::~TensorState()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	vkFreeCommandBuffers(device, command_pool, 1, &transfer_buffer);
	vkDestroyCommandPool(device, command_pool, nullptr);
	if (primary_buffer) {
		vkDestroyBuffer(device, primary_buffer.value(), nullptr);
	}
	if (staging_buffer) {
		vkDestroyBuffer(device, staging_buffer.value(), nullptr);
	}
	if (primary_memory) {
		vkFreeMemory(device, primary_memory.value(), nullptr);
	}
	if (staging_memory) {
		vkFreeMemory(device, staging_memory.value(), nullptr);
	}
}

VulkanTensor::VulkanTensor(TensorBuilder builder)
{
	state = std::make_shared<TensorState>(builder.device, builder.cache);
	state->shape = builder.shape;
	state->dtype = builder.dtype;
	state->dshare = builder.dshare;
	state->command_pool = create_command_pool(state->device, state->device.queue_family_index());
	state->transfer_buffer = create_command_buffers(state->device, state->command_pool, 1)[0];
	create_primary_buffer();
	create_staging_buffer();
}

std::size_t VulkanTensor::index(std::vector<std::size_t> indices)
{
	assert(indices.size() == state->shape.size());
	std::size_t flat_idx = 0;
	std::size_t stride = 1;
	for (std::size_t i = indices.size(); i > 0; i--) {
		std::size_t idx = indices[i - 1];
		flat_idx += idx * stride;
		stride *= state->shape[i - 1];
	}
	return flat_idx;
}

std::size_t VulkanTensor::get_byte_size(int32_t axis) const
{
	std::size_t size = [&](){
		switch (state->dtype) {
		case eBool: return 1;
		case eByte: return 1;
		case eInt32: return 4;
		case eInt64: return 8;
		case eFloat: return 4;
		case eDouble: return 8;
		default:
			throw std::runtime_error("MemoryDataType unsupported!");
		}
	}();
	return size * numel(axis);
}

std::size_t VulkanTensor::numel(int32_t axis) const
{
	std::size_t size = 1;
	if (axis >= 0) {
		return state->shape[axis] * size;
	}
	for (std::size_t dim : state->shape) {
		size *= dim;
	}
	return size;
}

VkBuffer VulkanTensor::get_buffer() const
{
	return state->primary_buffer.value();
}

VulkanTensor::TensorBuilder VulkanTensor::get_builder() const
{
	VulkanTensor::TensorBuilder builder = {
		.device = state->device,
		.cache = state->cache,
		.shape = state->shape,
		.dtype = state->dtype,
		.dshare = state->dshare
	};
	return builder;
}

std::vector<std::size_t> VulkanTensor::get_shape() const
{
	return state->shape;
}

VulkanTensor::MemoryDataType VulkanTensor::get_dtype() const
{
	return state->dtype;
}

VulkanTensor::MemorySharing VulkanTensor::get_dshare() const
{
	return state->dshare;
}

void VulkanTensor::create_primary_buffer()
{
	VkBufferUsageFlags usage = 0;
	usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	if (state->dshare == eUnique) {
		usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	VkMemoryPropertyFlags properties = 0;
	properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	if (state->dshare == eShared) {
		properties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	}

	VkBuffer buffer;
	VkDeviceMemory memory;
	
	create_buffer(
		buffer,
		memory,
		usage,
		properties
	);

	state->primary_buffer = buffer;
	state->primary_memory = memory;
}

void VulkanTensor::create_staging_buffer()
{
	if (state->dshare == eShared) {
		return; // No staging buffer needed.
	}

	VkBufferUsageFlags usage = 0;
	usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VkMemoryPropertyFlags properties = 0;
	properties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	properties |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	VkBuffer buffer;
	VkDeviceMemory memory;
	
	create_buffer(
		buffer,
		memory,
		usage,
		properties
	);

	state->staging_buffer = buffer;
	state->staging_memory = memory;
}

void VulkanTensor::create_buffer(VkBuffer& buffer, VkDeviceMemory& memory, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = align_modulo(get_byte_size(), INTERNAL_MEMORY_ALIGNMENT_BITSIZE);
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	CHECK_VULKAN(vkCreateBuffer(state->device, &buffer_info, nullptr, &buffer));

	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(state->device, buffer, &requirements);

	VkMemoryAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.allocationSize = requirements.size;
	allocate_info.memoryTypeIndex = find_memory_type(state->device, requirements.memoryTypeBits, properties);
	CHECK_VULKAN(vkAllocateMemory(state->device, &allocate_info, nullptr, &memory));

	vkBindBufferMemory(state->device, buffer, memory, 0);
}

void VulkanTensor::copy_buffer(VkBuffer source_buffer, VkBuffer target_buffer, VkBufferCopy config)
{
	CHECK_VULKAN(vkDeviceWaitIdle(state->device));

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CHECK_VULKAN(vkBeginCommandBuffer(state->transfer_buffer, &begin_info));
	vkCmdCopyBuffer(state->transfer_buffer, source_buffer, target_buffer, 1, &config);
	vkEndCommandBuffer(state->transfer_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &state->transfer_buffer;
	CHECK_VULKAN(vkQueueSubmit(state->device.queues()[0], 1, &submit_info, VK_NULL_HANDLE));
	
	CHECK_VULKAN(vkDeviceWaitIdle(state->device));
}

void VulkanTensor::update_descriptor(VkDescriptorSet descriptor, VkDescriptorType type, uint32_t binding)
{
	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = get_buffer();
	buffer_info.offset = 0;
	buffer_info.range = VK_WHOLE_SIZE;

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

	vkUpdateDescriptorSets(state->device, 1, &descriptor_write, 0, nullptr);
}

VulkanTensor VulkanTensor::add(VulkanTensor tensorA, VulkanTensor tensorB)
{
	/*
	TODO:
	Most of the stuff inside this function does not have to be
	specified here. For example each shader will have some uniform constants,
	so we could use one tensor for that and not allocate it every operator.
	Also all the stuff with the descriptor initialization.
	*/
	assert(tensorA.get_shape() == tensorB.get_shape());
	assert(tensorA.get_dtype() == tensorB.get_dtype());
	VulkanTensor output(tensorA.get_builder());
	auto& device = tensorA.state->device;
	auto& cache = tensorA.state->cache;

	VulkanTensor constants = {{
		.device = device,
		.cache = cache,
		.shape = {4 * 3},
		.dtype = VulkanTensor::eByte,
		.dshare = VulkanTensor::eShared
	}};
	constants.with_maps<uint32_t>([&](auto& values) {
		values[0] = tensorA.numel();
	});
	constants.with_maps<float>([&](auto& values) {
		values[1] = 1.0f;
		values[2] = 1.0f;
	});

	if (!cache.contains("vt_linear")) {
		cache.emplace("vt_linear", VulkanPipeline::from_comp_file(device, "assets/shaders/vt_linear.comp.spv"));
	}
	VulkanPipeline shader = cache.at("vt_linear");
	VkPipelineLayout shader_layout = shader.get_pipeline_layout();
	VkDescriptorPool descriptor_pool = create_descriptor_pool(device);
	VkDescriptorSetLayout descriptor_layout = shader.get_descriptor_layout();
	VkDescriptorSet descriptor = create_descriptor_sets(device, descriptor_pool, descriptor_layout, 1)[0];

	device.execute([&](VkCommandBuffer buffer) {
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, shader);
		constants.update_descriptor(descriptor, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
		tensorA.update_descriptor(descriptor, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1);
		tensorB.update_descriptor(descriptor, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2);
		output.update_descriptor(descriptor, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3);
		vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, shader_layout, 0, 1, &descriptor, 0, nullptr);
		std::size_t count = std::ceil(tensorA.numel() / 64.0f);
		vkCmdDispatch(buffer, count, 8, 8);
	});

	CHECK_VULKAN(vkDeviceWaitIdle(device));
	vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
	return output;
}

VulkanTensor VulkanTensor::add(VulkanTensor tensorA, float scalar)
{
	VulkanTensor tensorB(tensorA.get_builder());
	vt::fill(tensorB, scalar);
	return add(tensorA, tensorB);
}

VulkanTensor VulkanTensor::mul(VulkanTensor tensorA, VulkanTensor tensorB)
{
	assert(tensorA.get_shape() == tensorB.get_shape());
	assert(tensorA.get_dtype() == tensorB.get_dtype());
	VulkanTensor tensorZ(tensorA.get_builder());
	tensorA.with_maps<float>([&](std::vector<float>& valuesA) {
		tensorB.with_maps<float>([&](std::vector<float>& valuesB) {
			tensorZ.with_maps<float>([&](std::vector<float>& valuesZ) {
				for (std::size_t i = 0; i < valuesA.size(); i++) {
					valuesZ[i] = valuesA[i] * valuesB[i];
				}
			});
		});
	});
	return tensorZ;
}

VulkanTensor VulkanTensor::mul(VulkanTensor tensorA, float scalar)
{
	VulkanTensor tensorB(tensorA.get_builder());
	vt::fill(tensorB, scalar);
	return mul(tensorA, tensorB);
}

VulkanTensor VulkanTensor::pow(VulkanTensor tensorA, uint32_t exponent)
{
	VulkanTensor tensorZ(tensorA.get_builder());
	tensorA.with_maps<float>([&](std::vector<float>& valuesA) {
		tensorZ.with_maps<float>([&](std::vector<float>& valuesZ) {
			for (std::size_t i = 0; i < valuesA.size(); i++) {
				valuesZ[i] = std::pow(valuesA[i], exponent);
			}
		});
	});
	return tensorZ;
}

VulkanTensor operator+(VulkanTensor tensorA, VulkanTensor tensorB)
{
	return VulkanTensor::add(tensorA, tensorB);
}

VulkanTensor operator+(VulkanTensor tensorA, float scalar)
{
	return VulkanTensor::add(tensorA, scalar);
}

VulkanTensor operator+(float scalar, VulkanTensor tensorA)
{
	return VulkanTensor::add(tensorA, scalar);
}

VulkanTensor operator*(VulkanTensor tensorA, VulkanTensor tensorB)
{
	return VulkanTensor::mul(tensorA, tensorB);
}

VulkanTensor operator*(VulkanTensor tensorA, float scalar)
{
	return VulkanTensor::mul(tensorA, scalar);
}

VulkanTensor operator*(float scalar, VulkanTensor tensorA)
{
	return VulkanTensor::mul(tensorA, scalar);
}

}

#endif // INCLUDE_TENSOR_INLINE_HEADER
