
void RenderModule::attach(Family)
{
	createSwapChain();
	createImageViews();
	createFramebuffers();

	createRenderPass();
	createGraphicsPipeline();
	createCommandPool();
	createCommandBuffer();
	createSyncObjects();
	example = GetUnitCube();
	uint64_t size = sizeof(example.vertices[0]) * example.vertices.size();
	createVertexBuffer(&vertexBuffer, size);
	createVertexBuffer(&colorBuffer, size);
}

void RenderModule::detach(Family)
{
	vkDeviceWaitIdle(device);
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkDestroyBuffer(device, colorBuffer, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroyFence(device, inFlightFence, nullptr);
	vkDestroyCommandPool(device, commandPool, nullptr);
	for (auto framebuffer : framebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
}

void RenderModule::update(Family)
{
	auto result = vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
	VERIFY_VULKAN_RESULT(result);
	result = vkResetFences(device, 1, &inFlightFence);
	VERIFY_VULKAN_RESULT(result);
	uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	vkResetCommandBuffer(commandBuffer, 0);
	recordCommandBuffer(commandBuffer, imageIndex);
	submitCommandBuffer(commandBuffer);
	presentImage(imageIndex);
}

void RenderModule::createRenderPass()
{
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = imageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	auto result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
	VERIFY_VULKAN_RESULT(result);
}

void RenderModule::createGraphicsPipeline()
{
	graphicsPipeline = CreatePipelineForPolygon(device, renderPass);
}

void RenderModule::createCommandPool()
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndex;

	auto result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
	VERIFY_VULKAN_RESULT(result);
}

void RenderModule::createCommandBuffer()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	auto result = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
	VERIFY_VULKAN_RESULT(result);
}

void RenderModule::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	auto result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	VERIFY_VULKAN_RESULT(result);

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = imageExtent;

	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(imageExtent.width);
	viewport.height = static_cast<float>(imageExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = imageExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);
	result = vkEndCommandBuffer(commandBuffer);
	VERIFY_VULKAN_RESULT(result);
}

void RenderModule::createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	auto result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore);
	VERIFY_VULKAN_RESULT(result);

	result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore);
	VERIFY_VULKAN_RESULT(result);

	result = vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence);
	VERIFY_VULKAN_RESULT(result);
}

void RenderModule::presentImage(uint32_t imageIndex)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;

	VkSwapchainKHR swapchains[] = {swapchain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(presentQueue, &presentInfo);
}

void RenderModule::submitCommandBuffer(VkCommandBuffer commandBuffer)
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	auto result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence);
	VERIFY_VULKAN_RESULT(result);
}

void RenderModule::createVertexBuffer(VkBuffer* result, uint64_t size)
{
	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto success = vkCreateBuffer(device, &createInfo, nullptr, result);
	VERIFY_VULKAN_RESULT(success);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, *result, &memRequirements);
}
