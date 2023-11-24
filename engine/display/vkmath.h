#pragma once
#include "engine/display/vkinit.h"


namespace kodanuki::vkmath
{

// TODO: implement this and the rest of the new tensor class
template <typename T>
struct Expression
{

};

/**
 * Tensors are multi-dimensional arrays allowing for algebraic operations.
 *
 *
 * Example
 * -------
 * Tensor tensorA = Tensor::zeros({3, 4, 5});
 * Tensor tensorB = Tensor::zeros({3, 4, 5}) + 5.0f;
 * Tensor tensorC = 3.0f * tensorA + sin(tensorB);
 *
 * Here we create 3 tensors with 60 elements each. The tensorA only contains
 * the value zero and tensorB contains the value five. We got the tensorC by
 * evaluation the expression on the right.
 *
 *
 * Memory Model
 * ------------
 * We use the same memory model as Rust.
 *
 * Each tensor owns its memory during its lifetime. The memory is allocated
 * on first creation, and freed once it goes out of scope. Only one tensors
 * owns their memory and only one valid variable describes them. Invoking the
 * copy constructor allocates additional memory, and moving the tensors makes
 * previous variable invalid.
 *
 * We overload the operator& to simulate Rust's references. There will be a
 * const and non const version. The operator returns a reference object that
 * keeps count of the number of const and non const references.
 *
 * Thus the following properties are adhered:
 *     - exactly one tensor owns the underlying memory
 *     - at most one non const reference or any const references
 *     - moving is only allowed when there are no references
 *
 * These properties ensure that we cannot write to tensors when others are
 * reading from them, and we don't read while others are writing. Many reads
 * when no writing is happening is allowed.
 *
 * The user must ensure these rules, violations will yield in runtime errors.
 *
 *
 * Execution Model
 * ---------------
 * All expressions are executed asynchronous using the vulkan API.
 *
 * An expression takes the form "out = function(ins)" where "ins" are any
 * number of input tensors and "out" is one output tensor. In the example
 * we only have one expression, despite using multiple vkmath operations.
 *
 * Each expression is compiled to a shader as a whole and submitted to the
 * GPU. This means even large expressions are parallelized and we avoid the
 * many temporaries problem. We are however allowed to break this rule for
 * performance, e.g. when expressions are getting large.
 *
 * To ensure the memory model, we execute expressions that would break them
 * after previous ones. Otherwise the driver is free to execute expressions
 * in any order. This ensures memory integrity during execution.
 *
 * We wait implicitely for the results when we want to use tensor values
 * outside of the vkmath context.
 */
struct Tensor : public Expression<Tensor>
{
public:
	/**
	 * The available tensor types (only GLSL types can be supported).
	 */
	enum TensorType {eBool, eInt32, eUint32, eFloat, eDouble};

	/**
	 * The access type for write/read operations on the tensor.
	 */
	enum AccessType {eHostRead, eHostWrite, eDeviceRead, eDeviceWrite};

	/**
	 * Creates a tensor with undefined values.
	 *
	 * @param shape The dimensions of the tensor.
	 * @param dtype The internal type of the memory.
	 * @param usage The usage flags for the memory.
	 * @throw on vulkan errors during memory allocation.
	 */
	static Tensor empty(
		std::vector<std::size_t> shape,
		TensorType               dtype = eFloat,
		VkBufferUsageFlags       usage = {});

	/**
	 * Creates a tensor with zero values.
	 *
	 * @param shape The dimensions of the tensor.
	 * @param dtype The internal type of the memory.
	 * @param usage The usage flags for the memory.
	 * @throw on vulkan errors during memory allocation.
	 */
	static Tensor zeros(
		std::vector<std::size_t> shape,
		TensorType               dtype = eFloat,
		VkBufferUsageFlags       usage = {});

	/**
	 * Explicitely deleled constructor to avoid invalid tensors.
	 */
	Tensor() = delete;

	/**
	 * Creates a tensor with empty values (same as Tensor::empty()).
	 *
	 * @param shape The dimensions of the tensor.
	 * @param dtype The internal type of the memory.
	 * @param usage The usage flags for the memory.
	 * @throw on vulkan errors during memory allocation.
	 */
	Tensor(
		std::vector<std::size_t> shape,
		TensorType               dtype = eFloat,
		VkBufferUsageFlags       usage = {});

	/**
	 * The copy constructor copies the underlying memory.
	 * The tensor will have the same shape, dtype, and usage flags.
	 *
	 * @param other The other tensor used for copying.
	 * @throw on vulkan errors during memory allocation.
	 */
	Tensor(const Tensor& other);

	/**
	 * The copy operator copies the underlying memory.
	 * The tensor will keep the original dtype and usage flags.
	 *
	 * @param other The other tensor used for copying.
	 * @throw on vulkan errors during memory allocation.
	 */
	void operator=(const Tensor& other);

	/**
	 * The move operator basically renamed the tensor.
	 * The other tensor will move ownership of the memory to here.
	 *
	 * @param other The other tensor used for moving.
	 */
	Tensor(Tensor&& other) noexcept;

	/**
	 * The move operator basically renamed the tensor.
	 * The other tensor will move ownership of the memory to here.
	 *
	 * @param other The other tensor used for moving.
	 */
	void operator=(Tensor&& other) noexcept;

	/**
	 * Borrows the given tensor for writing purposes.
	 *
	 * @throw on validation of the Rust borrowing rules.
	 */
	Tensor& operator&();

	/**
	 * Borrows the given tensor for read only purposes.
	 *
	 * @throw on validation of the Rust borrowing rules.
	 */
	const Tensor& operator&() const;

	/**
	 * Returns the underlying memory as vector.
	 *
	 * This will wait on all operation involving this tensor to finish.
	 * It then maps the values from GPU memory to local memory and then
	 * executes the callback method with these values.
	 *
	 * Afterwards, copies the modified values back to GPU memory.
	 *
	 * @param callback The function to write internal values.
	 * @param element_offset The first element after linearization.
	 * @param element_number The number of mapped elements, zero for all.
	 * @throw on vulkan errors during mapping.
	 */
	template <typename T>
	void with_data(
		std::function<void(std::vector<T>&)> callback,
		uint32_t                             element_offset = 0,
		uint32_t                             element_number = 0)
	{
		sync_wait(eHostWrite);
		sync_load(sizeof(T) * element_offset, sizeof(T) * element_number);
		with_maps(callback, element_offset, element_number);
		sync_save(sizeof(T) * element_offset, sizeof(T) * element_number);
	}

	/**
	 * Returns the underlying memory as vector.
	 *
	 * This will wait on all operation involving this tensor to finish.
	 * It then maps the values from GPU memory to local memory and then
	 * executes the callback method with these values.
	 *
	 * Don't copies values back since they cannot be modified.
	 *
	 * @param callback The function to read internal values.
	 * @param element_offset The first element after linearization.
	 * @param element_number The number of mapped elements, zero for all.
	 * @throw on vulkan errors during mapping.
	 */
	template <typename T>
	void with_data(
		std::function<void(const std::vector<T>&)> callback,
		uint32_t                                   element_offset = 0,
		uint32_t                                   element_number = 0)
	{
		sync_wait(eHostRead);
		sync_load(sizeof(T) * element_offset, sizeof(T) * element_number);
		with_maps(callback, element_offset, element_number);
	}

private:
	/**
	 * Returns the number of elements inside this tensor. 
	 */
	uint32_t numel() const;

	/**
	 * Waits for operations to finish on the device.
	 *
	 * @param access The requested access on which to wait.
	 */
	void sync_wait(AccessType access);

	/**
	 * Copies the primary buffer to the staging buffer.
	 *
	 * @param byte_offset The position offset of the first copied byte.
	 * @param byte_number The number of bytes copied after the offset.
	 */
	void sync_load(uint32_t byte_offset, uint32_t byte_number);

	/**
	 * Copies the staging buffer to the primary buffer.
	 *
	 * @param byte_offset The position offset of the first copied byte.
	 * @param byte_number The number of bytes copied after the offset.
	 */
	void sync_save(uint32_t byte_offset, uint32_t byte_number);

	/**
	 * Maps the staging memory memory to a vector and executes the
	 * callback method for this vector.
	 *
	 * @param callback The function to read internal values.
	 * @param element_offset The first element after linearization.
	 * @param element_number The number of mapped elements, zero for all.
	 * @throw on vulkan errors during mapping.
	 */
	template <typename T>
	void with_maps(
		std::function<void(std::vector<T>&)> callback,
		uint32_t                             element_offset = 0,
		uint32_t                             element_number = 0)
	{
		if (element_number == 0) {
			element_number = numel() - element_offset;
		}
		T* data = static_cast<T*>(staging_memory) + element_offset;
		std::vector<T> elements(data, data + element_number);
		callback(elements);
	}

private:
	VulkanDevice                     device;
	vktype::buffer_t                 primary_buffer;
	vktype::buffer_t                 staging_buffer;
	void*                            staging_memory;
	std::vector<std::size_t>         shape;
	VkBufferUsageFlags               usage;
	vktype::fence_t                  cpu_write_lock;
	vktype::semaphore_t              gpu_usage_lock;
	std::vector<vktype::semaphore_t> gpu_read_locks;
	vktype::command_buffer_t         transfer_buffer;
	bool                             staging_equals_primary;
};

/**
 * Computes "Z[i] = A[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor id(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = abs(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor abs(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = sign(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor sign(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = ceil(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor ceil(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = floor(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor floor(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = round(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor round(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = clamp(A[i], B[i], C[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor clamp(VulkanTensor tensorA, VulkanTensor tensorB, VulkanTensor tensorC, bool inplace = false);

/**
 * Computes "Z[i] = clamp(A[i], b, c)".
 * If inplace is true then Z == A.
 */
VulkanTensor clamp(VulkanTensor tensorA, float constB, float constC, bool inplace = false);

/**
 * Computes "Z[i] = min(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor min(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = min(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor min(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = max(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor max(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = max(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor max(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] + B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor add(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] + b".
 * If inplace is true then Z == A.
 */
VulkanTensor add(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] - B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor sub(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] - b".
 * If inplace is true then Z == A.
 */
VulkanTensor sub(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] * B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor mul(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] * b".
 * If inplace is true then Z == A.
 */
VulkanTensor mul(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] / B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor div(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] / b".
 * If inplace is true then Z == A.
 */
VulkanTensor div(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = alpha * A[i] + beta * B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor linear(VulkanTensor tensorA, VulkanTensor tensorB, float alpha, float beta, bool inplace = false);

/**
 * Computes "Z[i] = alpha * A[i] + beta".
 * If inplace is true then Z == A.
 */
VulkanTensor linear(VulkanTensor tensorA, float alpha, float beta, bool inplace = false);

/**
 * Computes "Z[i] = A[i] == B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor eq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] == b".
 * If inplace is true then Z == A.
 */
VulkanTensor eq(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] > B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor gt(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] > b".
 * If inplace is true then Z == A.
 */
VulkanTensor gt(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] >= B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor geq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] >= b".
 * If inplace is true then Z == A.
 */
VulkanTensor geq(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] < B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor lt(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] < b".
 * If inplace is true then Z == A.
 */
VulkanTensor lt(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] <= B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor leq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] <= b".
 * If inplace is true then Z == A.
 */
VulkanTensor leq(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = sin(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor sin(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = cos(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor cos(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = tan(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor tan(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = exp(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor exp(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = log(A[i])" with base e.
 * If inplace is true then Z == A.
 */
VulkanTensor log(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = pow(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor pow(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = pow(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor pow(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "A[i] = range(start, stop)[i]".
 * Always inplace, output is the input tensor.
 */
VulkanTensor range(VulkanTensor tensorA, float start, float step);

/**
 * Computes "A[i] = b".
 * Always inplace, output is the input tensor.
 */
VulkanTensor fill(VulkanTensor tensorA, float constB);

}
