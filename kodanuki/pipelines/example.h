#pragma once
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/renderpass.h"
#include "plugin/vulkan/pipeline.h"
using namespace Kodanuki;

// Creates the renderpass that describes the rendering of the example triangle.
VulkanRenderpass get_example_triangle_renderpass(VulkanDevice device, VulkanSwapchain swapchain);

// Creates the pipeline that renders the example triangle.
VulkanPipeline get_example_triangle_pipeline(VulkanDevice device, VulkanRenderpass renderpass);
