#ifdef INCLUDE_TENSOR_INLINE_HEADER
#include "engine/vulkan/debug.h"
#include <optional>
#include <iostream>

namespace kodanuki
{

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
void VulkanTensor::with_maps(std::function<void(std::vector<T>&)> callback)
{
	with_mapped_memory<T>([&](T* data){
		if (state->dshare == eUnique) {
			VkBufferCopy config = {0, 0, get_byte_size()};
			copy_buffer(state->primary_buffer.value(), state->staging_buffer.value(), config);
		}

		std::vector<T> values(data, data + get_byte_size() / sizeof(T));
		callback(values);
		std::copy(values.begin(), values.end(), data);

		if (state->dshare == eUnique) {
			VkBufferCopy config = {0, 0, get_byte_size()};
			copy_buffer(state->staging_buffer.value(), state->primary_buffer.value(), config);
		}
	});
}

template <typename T>
void VulkanTensor::with_mapped_memory(std::function<void(T*)> callback)
{
	VkDeviceMemory memory = [&](){
		switch (state->dshare) {
		case eUnique: return state->staging_memory.value();
		case eShared: return state->primary_memory.value();
		}
		throw std::runtime_error("MemorySharing unsupported!"); 
	}();

	void* data;
	CHECK_VULKAN(vkMapMemory(state->device, memory, 0, get_byte_size(), 0, &data));
	T* typed_data = static_cast<T*>(data);
	callback(typed_data);
	vkUnmapMemory(state->device, memory);
}

template <typename T>
void VulkanTensor::execute(const Operator<VulkanTensor, T>& ops)
{
	/*
	The slow operations are really really slow, but they are much easier to
	implement. Slow operations are executed on the CPU but use the GPU memory.
	The faster way would be to implement them using vulkan compute shaders.
	
	We do this for now to have something working and start using it. This will
	yield a big performance hit, but this can be changed later.
	 */
	switch (ops.get_tensor(0).get_dtype()) {
	case eFloat:
		switch (ops.get_type()) {
		case OperatorType::eLinear:
			slow_execute_float_linear(ops);
			break;
		case OperatorType::eFill:
			slow_execute_float_fill(ops);
			break;
		default:
			throw std::runtime_error("Operator not supported!");
		}
		break;
	default:
		throw std::runtime_error("Operator not supported!");
	}
}

template <typename T>
void VulkanTensor::slow_execute_float_linear(const Operator<VulkanTensor, T>& ops)
{
	VulkanTensor tensorA = ops.get_tensor(0);
	VulkanTensor tensorB = ops.get_tensor(1);
	VulkanTensor tensorC = ops.get_tensor(2);
	T alpha = ops.get_constant(0);
	T beta = ops.get_constant(1);

	tensorA.with_maps<T>([&](std::vector<T>& valuesA) {
		tensorB.with_maps<T>([&](std::vector<T>& valuesB) {
			tensorC.with_maps<T>([&](std::vector<T>& valuesC) {
				for (std::size_t i = 0; i < valuesA.size(); i++) {
					valuesC[i] = alpha * valuesA[i] + beta * valuesB[i];
				}
			});
		});
	});
}

template <typename T>
void VulkanTensor::slow_execute_float_fill(const Operator<VulkanTensor, T>& ops)
{
	VulkanTensor tensor = ops.get_tensor(0);
	T fill_value = ops.get_constant(0);

	tensor.with_maps<T>([&](std::vector<T>& values){
		for (std::size_t i = 0; i < values.size(); i++) {
			values[i] = fill_value;
		}
	});
}

template <typename T>
void VulkanTensor::load_data(const std::vector<T> values, uint32_t offset)
{
	with_maps<T>([&](std::vector<T>& buffer) {
		uint32_t max_size = std::min(buffer.size() - offset, values.size());
		for (uint32_t i = 0; i < max_size; i++) {
			buffer[i + offset] = values[i];
		}
	});
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
	create_command_buffer();
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

std::size_t VulkanTensor::get_byte_size() const
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

void VulkanTensor::create_command_buffer()
{
	VkCommandPoolCreateInfo pool_info;
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.pNext = nullptr;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = state->device.queue_family_index();
	CHECK_VULKAN(vkCreateCommandPool(state->device, &pool_info, nullptr, &state->command_pool));

	VkCommandBufferAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate_info.pNext = nullptr;
	allocate_info.commandPool = state->command_pool;
	allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate_info.commandBufferCount = 1;
	CHECK_VULKAN(vkAllocateCommandBuffers(state->device, &allocate_info, &state->transfer_buffer));
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
	buffer_info.size = get_byte_size();
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

std::string VulkanTensor::get_shader_name(MemoryDataType dtype, OperatorType otype)
{
	std::stringstream ss;
	ss << "vt_";
	ss << [&](){
		switch (dtype) {
		case eFloat: return "float";
		default: throw std::runtime_error("Shader not implemented yet!"); 
		}
		
	}();
	ss << "_";
	ss << [&](){
		switch (otype) {
		case OperatorType::eLinear: return "linear";
		default: throw std::runtime_error("Shader not implemented yet!");
		}
	}();
	ss << ".comp";
	return ss.str();
}

}

#endif // INCLUDE_TENSOR_INLINE_HEADER
