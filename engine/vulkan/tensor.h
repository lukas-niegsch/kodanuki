#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/operator.h"
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
 * also free any allocated memory.
 */
class VulkanTensor : public op<VulkanTensor>
{
public:
	/**
 	 * An enum that specifies whether the memory of a tensor is stored on
	 * the device (GPU RAM) or host (CPU RAM). Device memory is slower for
	 * writing and reading, but much faster for processing.
	 */
	enum MemoryLocation
	{
		// The memory is located inside the device.
		eDevice,

		// The memory is located inside the host.
		eHost
	};

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

		// The shape of the tensor (row-major format).
		std::vector<std::size_t> shape;

		// The data type for the tensor memory.
		MemoryDataType dtype = eFloat;

		// The location for the tensor memory.
		MemoryLocation dside = eDevice;

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
	 * Converts the given indices to the index of the flattened tensor.
	 *
	 * This will throw an exception if the number of indices does not match
	 * the size of the tensor. This function assumes that the vector is
	 * given in row-major order, the last dimension is continous in memory.
	 *
	 * @param indices The indices for the different dimensions.
	 * @return The index of the element in the flattened tensor.
	 */
	template <typename ... Index>
	std::size_t index(std::tuple<Index...> indices);

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
	 */
	template <typename T>
	void with_maps(std::function<void(std::vector<T>&)> callback);

	/**
	 * Executes the given operator for the input tensors.
	 *
	 * The output tensors might be the same as the import tensors and the
	 * operation might be executed asynchronous. The only guarentee is that
	 * the order of operation based on their call time will be kept. It is
	 * also guarenteed the data stays consistent.
	 *
	 * @param ops The operator that should be executed.
	 * @param tensors The input tensors of the operation.
	 * @return The output tensors of the operation.
	 */
	static std::vector<VulkanTensor> execute(const Operator& ops, const std::vector<VulkanTensor> tensors);

public:
	/**
	 * Returns the vulkan buffer that was allocated for this tensor.
	 *
	 * @return The vulkan buffer that contains the flattened values.
	 */
	VkBuffer get_buffer() const;

	/**
	 * Returns the size of the tensor in bytes.
	 *
	 * @return The number of bytes inside the buffer.
	 */
	std::size_t get_byte_size() const;

	/**
	 * Returns the shape for this tensor.
	 *
	 * @return The size of each dimension.
	 */
	std::vector<std::size_t> get_shape() const;

	/**
	 * Returns the data type of the elements in this tensor.
	 *
	 * @return The data type of the tensor.
	 */
	MemoryDataType get_dtype() const;

	/**
	 * Returns the location of the memory used by this tensor.
	 *
	 * @return The memory location.
	 */
	MemoryLocation get_dside() const;

	/**
	 * Returns the sharing mode of the memory used by this tensor.
	 *
	 * @return The memory sharing mode.
	 */
	MemorySharing get_dshare() const;

private:
	// Shared state to automatically delete unused instances.
	std::shared_ptr<struct TensorState> state;
};

}
