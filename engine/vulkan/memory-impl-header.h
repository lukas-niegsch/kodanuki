
struct MemoryState
{
	VulkanDevice device;
	VkDeviceMemory memory;
	VkBuffer memory_buffer;
	uint32_t byte_size;
	~MemoryState();
};

struct StagingBufferState
{
	VulkanDevice device;
	VulkanMemory memory;
	uint32_t offset;
	uint32_t count;
	VkDeviceMemory staging_memory;
	VkBuffer staging_buffer;
	VkFence synchronization;
	void* data;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	inline static uint64_t TIMEOUT = 10 /* seconds*/ * 1000000000ull;
	~StagingBufferState();
};

void create_internal_buffer(VulkanDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory);
