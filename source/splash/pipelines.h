#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/pipeline.h"
#include "engine/vulkan/target.h"

namespace kodanuki
{

// The graphics pipeline that renders fluid particles.
VulkanPipeline create_render_fluid_pipeline(VulkanDeviceOld device, VulkanTarget target);

}
