#pragma once
#include "engine/display/vkinit.h"
#include <functional>


namespace kodanuki::vkutil
{

/**
 * Aquires the next frame for rendering.
 *
 * @param device The device that executes commands.
 * @param window The window that renders the frame.
 */
void aquire_frame(VulkanDevice device, VulkanWindow& window);

/**
 * Records the draw commands for the current frame.
 *
 * @param device The device that executes commands.
 * @param window The window that renders the frame.
 * @param commands The draw commands for recording.
 */
void record_frame(VulkanDevice device, VulkanWindow& window, std::vector<std::function<void(VkCommandBuffer)>> commands);

/**
 * Submits the commands for the current frame.
 *
 * @param device The device that executes commands.
 * @param window The window that renders the frame.
 * @param queue_index The device queue on which to submit commands.
 */
void submit_frame(VulkanDevice device, VulkanWindow& window, uint32_t queue_index = 0);

/**
 * Renders the current image onto the window surface.
 *
 * @param device The device that executes commands.
 * @param window The window that renders the frame.
 * @param queue_index The device queue on which to submit commands.
 */
void render_frame(VulkanDevice device, VulkanWindow& window, uint32_t queue_index = 0);

}
