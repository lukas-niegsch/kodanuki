#pragma once
#include <vulkan/vulkan.h>

namespace Kodanuki
{

/**
 * Creates a vulkan pipeline for a polygon objects.
 * 
 * Lots of these values are hard coded for now to start somewhere.
 */
VkPipeline CreatePipelineForPolygon(VkDevice device, VkRenderPass renderPass);

};
