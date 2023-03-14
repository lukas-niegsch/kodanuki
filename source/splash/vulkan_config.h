#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/pipeline.h"
#include "engine/vulkan/target.h"

namespace kodanuki
{

/**
 * Scores the different physical devices.
 * This will prefer external graphics cards.
 *
 * @param device The phyiscal device to score.
 * @return The score that the graphics card gets.
 */
int score_physical_device(VkPhysicalDevice device);

/**
 * Scores the different queue families for the selected physical device.
 * This will prefer the family who is able to process compute and graphics
 * commands.
 *
 * @param family The queue family to score.
 * @return The score that the queue family gets.
 */
int score_queue_family(VkQueueFamilyProperties family);

/**
 * Creates a simple renderpass.
 *
 * The renderpass does not allow for transparent objects and has a depth
 * buffer enabled. Otherwise we pretty much use the default configuration.
 *
 * @param device The device for which the renderpass will be created.
 * @param renderpass The resulting renderpass that will be created.
 */
void create_simple_renderpass(VkDevice device, VkRenderPass& renderpass);

/**
 * Creates a simple pipeline for rendering fluids.
 *
 * The pipeline uses instanced rendering where each instance only is specified
 * with some position. We don't include instance rotations since we only try
 * to render spheres. The pipeline also handles the shading of the spheres.
 *
 * @param device The device on which the pipeline will be created.
 * @param target The target on which the pipeline results will be rendered.
 * @return The complete graphics pipeline for SPH fluid rendering.
 */
VulkanPipeline create_render_fluid_pipeline(VulkanDevice device, VulkanTarget target);

}
