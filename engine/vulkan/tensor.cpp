#ifdef INCLUDE_TENSOR_INLINE_HEADER
#include "engine/vulkan/debug.h"
#include "engine/vulkan/wrapper.h"
#include <optional>
#include <iostream>
#include <cmath>
#include <cassert>

namespace kodanuki
{

struct TensorState
{
	VulkanContext device;
	VulkanPipelineOldCache& cache;
	std::vector<std::size_t> shape;
	VulkanTensor::MemoryDataType dtype;
	VulkanTensor::MemorySharing dshare;
	std::optional<VkBufferUsageFlags> usage;
	std::optional<VkBuffer> primary_buffer;
	std::optional<VkBuffer> staging_buffer;
	Wrapper<VkCommandBuffer> transfer_buffer;
	std::optional<Wrapper<VkDeviceMemory>> primary_memory;
	std::optional<Wrapper<VkDeviceMemory>> staging_memory;
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
	tensor.with_maps<T>([&](std::vector<T>& values){
		for (std::size_t i = 0; i < values.size(); i++) {
			values[i] = value;
		}
	});
}

}

#else // INCLUDE_TENSOR_INLINE_HEADER
#include "engine/vulkan/tensor.h"
#include "engine/utility/alignment.h"

namespace kodanuki
{

TensorState::~TensorState()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	transfer_buffer = {};
	if (primary_buffer) {
		vkDestroyBuffer(device, primary_buffer.value(), nullptr);
	}
	if (staging_buffer) {
		vkDestroyBuffer(device, staging_buffer.value(), nullptr);
	}
	primary_memory = {};
	staging_memory = {};
}

VulkanTensor::VulkanTensor(TensorBuilder builder)
{
	state = std::make_shared<TensorState>(builder.device, builder.cache);
	state->shape = builder.shape;
	state->dtype = builder.dtype;
	state->dshare = builder.dshare;
	state->transfer_buffer = create_command_buffer(state->device, state->device.get_command_pool());
	create_primary_buffer();
	create_staging_buffer();
}

VulkanTensor::VulkanTensor(const VulkanTensor& other)
{
	this->state = other.state;
}

VulkanTensor VulkanTensor::operator=(const VulkanTensor& other)
{
	this->state = other.state;
	return *this;
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
		.dshare = state->dshare,
		.usage = state->usage
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

	if (state->usage) {
		usage = state->usage.value();
	} else {
		usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}

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
	Wrapper<VkDeviceMemory> memory;
	
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
	Wrapper<VkDeviceMemory> memory;
	
	create_buffer(
		buffer,
		memory,
		usage,
		properties
	);

	state->staging_buffer = buffer;
	state->staging_memory = memory;
}

void VulkanTensor::create_buffer(VkBuffer& buffer, Wrapper<VkDeviceMemory>& memory, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = align_modulo(get_byte_size(), INTERNAL_MEMORY_ALIGNMENT_BITSIZE);
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	CHECK_VULKAN(vkCreateBuffer(state->device, &buffer_info, nullptr, &buffer));

	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(state->device, buffer, &requirements);
	memory = create_device_memory(
		state->device, state->device.get_physical_device(), requirements,
		properties
	);
	vkBindBufferMemory(state->device, buffer, memory, 0);
}

void VulkanTensor::copy_buffer(VkBuffer source_buffer, VkBuffer target_buffer, VkBufferCopy config)
{
	state->device.with_command_buffer(state->transfer_buffer, [&](VkCommandBuffer buffer) {
		vkCmdCopyBuffer(buffer, source_buffer, target_buffer, 1, &config);
	});

	VkCommandBuffer transfer_buffer = state->transfer_buffer;
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &transfer_buffer;
	CHECK_VULKAN(vkQueueSubmit(state->device.get_queues()[0], 1, &submit_info, VK_NULL_HANDLE));
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

void VulkanTensor::execute(std::string name, std::vector<VulkanTensor> tensors, std::vector<float> constants, bool update_descriptor)
{
	assert(!tensors.empty());
	auto& device = tensors[0].state->device;
	auto& cache = tensors[0].state->cache;

	if (!cache.contains(name)) {
		std::string filename = std::string("assets/shaders/") + name + ".comp.spv";
		cache.emplace(name, VulkanPipelineOld::from_comp_file(device, filename));
	}
	VulkanPipelineOld shader = cache.at(name);

	// The first element of each push_constant block will be the tensor size.
	uint32_t count = tensors[0].numel();
	constants.insert(constants.begin(), std::bit_cast<float>(count));

	VkPipelineLayout shader_layout = shader.get_pipeline_layout();
	VkDescriptorSet descriptor = shader.get_primary_descriptor();

	if (update_descriptor) {
		for (uint32_t i = 0; i < tensors.size(); i++) {
			tensors[i].update_descriptor(descriptor, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, i);
		}
	}

	float elapsed_nanoseconds = device.execute([&](VkCommandBuffer buffer) {
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, shader);
		vkCmdPushConstants(buffer, shader_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float) * align_modulo(constants.size(), 4), constants.data());
		vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, shader_layout, 0, 1, &descriptor, 0, nullptr);
		vkCmdDispatch(buffer, (tensors[0].numel() + 63) / 64, 1, 1);
		vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 0, nullptr);
	});

	(void) elapsed_nanoseconds;
	// std::cout << "VT " << name << " elapsed time: " << elapsed_nanoseconds << "ns" << '\n';
}

VulkanTensor VulkanTensor::add(VulkanTensor tensorA, VulkanTensor tensorB, bool update_descriptor)
{
	VulkanTensor tensorZ(tensorA.get_builder());
	execute("vt_add", {tensorZ, tensorA, tensorB}, {}, update_descriptor);
	return tensorZ;
}

void VulkanTensor::add_i(VulkanTensor tensorZ, VulkanTensor tensorA, bool update_descriptor)
{
	execute("vt_add_i", {tensorZ, tensorA}, {}, update_descriptor);
}

VulkanTensor VulkanTensor::add(VulkanTensor tensorA, float scalar, bool update_descriptor)
{
	VulkanTensor tensorZ(tensorA.get_builder());
	execute("vt_add_c", {tensorZ, tensorA}, {scalar}, update_descriptor);
	return tensorZ;
}

void VulkanTensor::add_i(VulkanTensor tensorZ, float scalar, bool update_descriptor)
{
	execute("vt_add_ic", {tensorZ}, {scalar}, update_descriptor);
}

VulkanTensor VulkanTensor::mul(VulkanTensor tensorA, VulkanTensor tensorB, bool update_descriptor)
{
	VulkanTensor tensorZ(tensorA.get_builder());
	execute("vt_mul", {tensorZ, tensorA, tensorB}, {}, update_descriptor);
	return tensorZ;
}

void VulkanTensor::mul_i(VulkanTensor tensorZ, VulkanTensor tensorA, bool update_descriptor)
{
	execute("vt_mul_i", {tensorZ, tensorA}, {}, update_descriptor);
}

VulkanTensor VulkanTensor::mul(VulkanTensor tensorA, float scalar, bool update_descriptor)
{
	VulkanTensor tensorZ(tensorA.get_builder());
	execute("vt_mul_c", {tensorZ, tensorA}, {scalar}, update_descriptor);
	return tensorZ;
}

void VulkanTensor::mul_i(VulkanTensor tensorZ, float scalar, bool update_descriptor)
{
	execute("vt_mul_ic", {tensorZ}, {scalar}, update_descriptor);
}

VulkanTensor VulkanTensor::pow(VulkanTensor tensorA, float exponent, bool update_descriptor)
{
	VulkanTensor tensorZ(tensorA.get_builder());
	execute("vt_pow", {tensorZ, tensorA}, {exponent}, update_descriptor);
	return tensorZ;
}

void VulkanTensor::pow_i(VulkanTensor tensorZ, float exponent, bool update_descriptor)
{
	execute("vt_pow_i", {tensorZ}, {exponent}, update_descriptor);
}

VulkanTensor VulkanTensor::copy(VulkanTensor tensorA, bool update_descriptor)
{
	VulkanTensor tensorZ(tensorA.get_builder());
	execute("vt_copy", {tensorZ, tensorA}, {}, update_descriptor);
	return tensorZ;
}

void VulkanTensor::copy_i(VulkanTensor tensorZ, VulkanTensor tensorA, bool update_descriptor)
{
	execute("vt_copy", {tensorZ, tensorA}, {}, update_descriptor);
}

VulkanTensor VulkanTensor::linear(float alpha, VulkanTensor tensorA, float beta, VulkanTensor tensorB, bool update_descriptor)
{
	VulkanTensor tensorZ(tensorA.get_builder());
	execute("vt_linear", {tensorZ, tensorA, tensorB}, {alpha, beta}, update_descriptor);
	return tensorZ;
}

void VulkanTensor::linear_i(float alpha, VulkanTensor tensorZ, float beta, VulkanTensor tensorA, bool update_descriptor)
{
	execute("vt_linear_i", {tensorZ, tensorA}, {alpha, beta}, update_descriptor);
}

}

#endif // INCLUDE_TENSOR_INLINE_HEADER
