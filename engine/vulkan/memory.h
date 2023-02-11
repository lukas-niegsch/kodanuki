#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/debug.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace kodanuki
{

/**
 * Contains all the configurable information for allocating memory.
 *
 * The user has to make sure that the allocation will succeed. The
 * GPU must have at least as much bytes of memory that are inside
 * one continuous block.
 */
struct MemoryBuilder
{
	// The vulkan device for which the memory is allocated.
	VulkanDevice device;

	// The number of bytes the memory should allocate.
	uint32_t byte_size;
};

/**
 * The vulkan memory wraps one continuous block in memory.
 *
 * The memory can only contain POD data. The type will be
 * erased, but can be recovered via the create_buffer()
 * method.
 *
 * The used memory is (probably) device local for optimal
 * performance, but we allow access to it via a staging
 * buffer. Reading and writing via this way is very slow
 * and should be avoided. The advantage is that shaders can
 * access this memory very fast.
 *
 * The size of the memory should not be too big, because
 * the allocation might fail. However, it should also not
 * be too small, because many graphics card limit the
 * number of allocations.
 * 
 * Instances can be copied around freely and will release
 * all ressources once unused. The staging buffer will be
 * deleted once it goes out of scope.
 */
class VulkanMemory
{
public:
	// Allocates new vulkan memory from the given builder.
	VulkanMemory(MemoryBuilder builder);

	// Returns the buffer containing all the used memory.
	operator VkBuffer();

public:
	// Returns the size of the memory in bytes.
	uint32_t get_byte_size();

public:
	/**
	 * The staging buffer maps the device local memory to
	 * host visible memory. This class should be created
	 * using the create_buffer() method.
	 *
	 * The push and pull commands copy the whole buffer to
	 * and from the device local memory. This operation is
	 * very slow. However copying one big buffer should be
	 * faster than copying many small ones.
	 *
	 * The staging buffer will be destroyed once unused, but
	 * the contents will not be explicitely pushed.
	 */
	template <typename T>
	struct StagingBuffer
	{
	public:
		// Creates a new staging buffer.
		StagingBuffer(VulkanDevice device, VulkanMemory memory, uint32_t offset, uint32_t count);
		
		// Returns the reference to the mapped element.
		T& operator[](uint32_t index);

		// Copies the data from the staging buffer to the memory.
		void slow_push();

		// Copies the data from the memory to the staging buffer.
		void slow_pull();

	private:
		// Copies the data from the src buffer to the dst buffer.
		void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkBufferCopy config);

	private:
		// The abstract pointer to the implementation.
		std::shared_ptr<class StagingBufferState> state;
	};

	// Creates a new stating buffer that points the the memory.
	struct CreateBufferParams
	{
		uint32_t offset;
		uint32_t count;
	};
	template <typename T>
	StagingBuffer<T> create_buffer(CreateBufferParams params);

private:
	// The abstract pointer to the implementation.
	std::shared_ptr<struct MemoryState> state;
};

#include "memory-impl-header.h"
#include "memory-impl-source.h"

}
