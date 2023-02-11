#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/pipeline.h"
#include "engine/vulkan/target.h"

namespace kodanuki
{

// The graphics pipeline that renders fluid particles.
VulkanPipeline create_render_fluid_pipeline(VulkanDevice device, VulkanTarget target);

// The compute pipeline that calculates the pressure.
VulkanPipeline create_update_fluid_pipeline_pressure(VulkanDevice device);

// The compute pipeline that calculates the position and velocity.
VulkanPipeline create_update_fluid_pipeline_simulate(VulkanDevice device);

}
