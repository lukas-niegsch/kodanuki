#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/pipeline.h"
#include "engine/utility/signature.h"
#include <vector>
#include <memory>
#include <functional>
#include <tuple>

namespace kodanuki
{

/**
 * The vulkan tensor is an tensor implementation that allows for GPU storage.
 * All the operations for this tensor are implemented using CRTP pattern. This
 * class uses a shared pointer to store it variables. That means that it can
 * be copied freely and will be deleted once it is no longer used. This will
 * also free any allocated memory. Currently all tensor operation will be
 * executed synchronously and wait for completion, this will be changed later.
 */
class VulkanTensor
{
public:
	/**
	 * An enum that specifies whether both device or host sees the memory
	 * or just one of them. Unique memory is much faster for processing but
	 * requires staging buffers for writing and reading.
	 */
	enum MemorySharing
	{
		// The memory can be seen by everybody.
		eShared,

		// The memory can only be seen inside the location.
		eUnique
	};

	/**
	 * An enum that specifies the type of the tensor. This type will be used
	 * for computations inside compute shaders.
	 */
	enum MemoryDataType
	{
		// 8 bit boolean type.
		eBool,

		// 8 bit byte type for custom types.
		eByte,

		// 32 bit integer type (signed).
		eInt32,

		// 64 bit integer type (signed).
		eInt64,

		// 32 bit single precision floating point number.
		eFloat,

		// 64 bit double precision floating point number.
		eDouble
	};

public:
	/**
	 * A builder struct for creating VulkanTensor objects.
	 *
	 * Note that setting wrong values during initialization might lead
	 * to unexpected results. Also make sure that the size of the tensor
	 * fits inside the device. Otherwise, an error will be thrown.
	 */
	struct TensorBuilder
	{
		// The vulkan device that holds the context.
		VulkanDevice device;

		// The vulkan pipeline cache for the compute pipelines.
		VulkanPipelineCache& cache;

		// The shape of the tensor (row-major format).
		std::vector<std::size_t> shape;

		// The data type for the tensor memory.
		MemoryDataType dtype = eFloat;

		// The sharing model for the tensor memory.
		MemorySharing dshare = eUnique;
	};

	/**
	 * Creates a new VulkanTensor from the given builder.
	 *
	 * @param builder The builder that contains the tensor properties.
	 */
	VulkanTensor(TensorBuilder builder);

public:
	/**
	 * Creates a new tensor containing the sum of the input tensors.
	 *
	 * @param tensorA The first summand tensor.
	 * @param tensorB The second summand tensor.
	 * @return The tensor containing the result.
	 */
	static VulkanTensor add(VulkanTensor tensorA, VulkanTensor tensorB);

	/**
	 * Fills the given tensor with some value.
	 *
	 * @param tensor The tensor to fill.
	 * @param value The value the tensor should contain afterwards.
	 */
	template <typename T>
	static void fill(VulkanTensor& tensor, const T& value);

public:
	/**
	 * Converts the given indices to the index of the flattened tensor.
	 *
	 * This will throw an exception if the number of indices does not match
	 * the size of the tensor. This function assumes that the vector is
	 * given in row-major order, the last dimension is continous in memory.
	 *
	 * @param indices The indices for the different dimensions.
	 * @return The index of the element in the flattened tensor.
	 */
	std::size_t index(std::vector<std::size_t> indices);

	/**
	 * Closure to let the user change the content of the tensor.
	 *
	 * The tensor data will be given as a flattened vector, use to index()
	 * function to access elements based on their position. The template
	 * type should match the dtype because the data will afterwards be
	 * interpreted as such.
	 *
	 * For unique device memory this will create a staging buffer and moves
	 * the data from and to the device. This makes this function inefficient
	 * for these cases.
	 *
	 * @param callback The function that will be execute to modify values.
	 * @param offset Skips the first bytes inside the flattended tensor.
	 */
	template <typename T>
	void with_maps(std::function<void(std::vector<T>&)> callback, uint32_t offset = 0);

public:
	/**
	 * Loads some values into the tensor.
	 *
	 * This is the same as replacing every entry inside the flattened tensor.
	 * Too little or too many values will be ignored, only valid memory will
	 * be loaded. This ignores the soft type checks and allows the used to
	 * specify arbitrary data. However, ideally the template type matches the
	 * dtype of the tensor.
	 *
	 * @param values The new values that this tensor should receive.
	 * @param offset Skip the first values inside the tensor.
	 */
	template <typename T>
	void load_data(const std::vector<T> values, uint32_t offset = 0);

public:
	/**
	 * Returns the primary buffer for this tensor.
	 *
	 * @return The primary buffer for this tensor.
	 */
	VkBuffer get_buffer() const;

	/**
	 * Returns the size of the tensor in bytes.
	 *
	 * @param axis The dimension on which to count or -1 for all.
	 * @return The number of bytes inside the buffer.
	 */
	std::size_t get_byte_size(int32_t axis = -1) const;

	/**
	 * Returns the parameters that are used to build this tensor.
	 * 
	 * @return The builder used to build this tensor.
	 */
	TensorBuilder get_builder() const;

	/**
	 * Returns the shape for this tensor.
	 *
	 * @return The size of each dimension.
	 */
	std::vector<std::size_t> get_shape() const;

	/**
	 * Returns the number of elements.
	 *
	 * @param axis The dimension on which to count or -1 for all.
	 * @return The number of elements.
	 */
	std::size_t numel(int32_t axis = -1) const;

	/**
	 * Returns the data type of the elements in this tensor.
	 *
	 * @return The data type of the tensor.
	 */
	MemoryDataType get_dtype() const;

	/**
	 * Returns the sharing mode of the memory used by this tensor.
	 *
	 * @return The memory sharing mode.
	 */
	MemorySharing get_dshare() const;

private:
	/**
	 * Creates the command pool and the command buffer.
	 *
	 * This command buffer will be used to transfer from the staging
	 * buffer to the primary buffer.
	 */
	void create_command_buffer();

	/**
	 * Creates the primary memory and primary buffer.
	 *
	 * The primary buffer will be used for all the operations. It will be
	 * faster storage, but might require a staging buffer for transfer.
	 */
	void create_primary_buffer();

	/**
	 * Creates the staging memory and staging buffer.
	 *
	 * The staging buffer will be used as tempory storage before transfer. It
	 * is host visible, but also significant slower than the primary buffer.
	 */
	void create_staging_buffer();

	/**
	 * Creates some memory and the corresponding buffer.
	 *
	 * The buffer will have the size defined by the get_byte_size() method.
	 * This will allocate memory and will fail if the device is out of memory.
	 *
	 * @param buffer The output buffer that will be created.
	 * @param memory The output memory that will be allocated.
	 * @param usage The usage flags that this buffer has.
	 * @param properties The property flags that this buffer has.
	 */
	void create_buffer(VkBuffer& buffer, VkDeviceMemory& memory, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	/**
	 * Copies the data from the source buffer to the target buffer.
	 * 
	 * @param source_buffer The buffer containing the source data.
	 * @param target_buffer The buffer containing the target data.
	 * @param config The configuration that will be used for copying. 
	 */
	void copy_buffer(VkBuffer source_buffer, VkBuffer target_buffer, VkBufferCopy config);

	/**
	 * Closure that executes the callback function with mapped memory.
	 *
	 * This might be the primary memory or the staging memory depending
	 * on the memory sharing model. Also casts the resulting void pointer
	 * to the given size.
	 *
	 * @param callback The callback function that will be executed.
	 * @param offset Skips the first bytes inside the memory.
	 */
	template <typename T>
	void with_mapped_memory(std::function<void(T*)> callback, uint32_t offset = 0);

private:
	// TODO: Implement compute shader version.
	template <typename T>
	static void slow_execute_linear(VulkanTensor tensorZ, T alpha, VulkanTensor tensorA, T beta, VulkanTensor tensorB);

	// TODO: Implement compute shader version.
	template <typename T>
	static void slow_execute_fill(VulkanTensor tensor, T value);

private:
	// Shared state to automatically delete unused instances.
	std::shared_ptr<struct TensorState> state;
};

// Shortcut for equations for a vulkan tensor.
using vt = VulkanTensor;

}

#define INCLUDE_TENSOR_INLINE_HEADER
#include "engine/vulkan/tensor.cpp"
