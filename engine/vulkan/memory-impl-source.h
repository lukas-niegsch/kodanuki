
template <typename T>
VulkanMemory::StagingBuffer<T> VulkanMemory::create_buffer(CreateBufferParams params)
{
	return StagingBuffer<T>({
		state->device,
		*this,
		params.offset,
		params.count
	});
}

template <typename T>
VulkanMemory::StagingBuffer<T>::StagingBuffer(VulkanDevice device, VulkanMemory memory, uint32_t offset, uint32_t count)
{
	state = std::make_shared<StagingBufferState>(device, memory, offset, count);

	create_internal_buffer(
		state->device,
		sizeof(T) * count,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		state->staging_buffer,
		state->staging_memory
	);

	VkCommandPoolCreateInfo pool_info;
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.pNext = nullptr;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = device.queue_family_index();
	CHECK_VULKAN(vkCreateCommandPool(device, &pool_info, nullptr, &state->command_pool));

	VkCommandBufferAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate_info.pNext = nullptr;
	allocate_info.commandPool = state->command_pool;
	allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate_info.commandBufferCount = 1;
    CHECK_VULKAN(vkAllocateCommandBuffers(device, &allocate_info, &state->command_buffer));

	vkMapMemory(state->device, state->staging_memory, 0, sizeof(T) * count, 0, &state->data); 
}

template <typename T>
T& VulkanMemory::StagingBuffer<T>::operator[](uint32_t index)
{
	CHECK_VULKAN(vkDeviceWaitIdle(state->device));
	return *(static_cast<T*>(state->data) + index);
}

template <typename T>
void VulkanMemory::StagingBuffer<T>::slow_push()
{
	VkBufferCopy config = {};
    config.srcOffset = 0;
    config.dstOffset = state->offset;
    config.size = sizeof(T) * state->count;
	copy_buffer(state->staging_buffer, state->memory, config);
}

template <typename T>
void VulkanMemory::StagingBuffer<T>::slow_pull()
{
	VkBufferCopy config = {};
    config.srcOffset = state->offset;
    config.dstOffset = 0;
    config.size = sizeof(T) * state->count;
	copy_buffer(state->memory, state->staging_buffer, config);
}

template <typename T>
void VulkanMemory::StagingBuffer<T>::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkBufferCopy config)
{
	CHECK_VULKAN(vkDeviceWaitIdle(state->device));
	
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CHECK_VULKAN(vkBeginCommandBuffer(state->command_buffer, &begin_info));
	vkCmdCopyBuffer(state->command_buffer, src_buffer, dst_buffer, 1, &config);
	vkEndCommandBuffer(state->command_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &state->command_buffer;
	CHECK_VULKAN(vkQueueSubmit(state->device.queues()[0], 1, &submit_info, VK_NULL_HANDLE));
	
	CHECK_VULKAN(vkDeviceWaitIdle(state->device));
}
