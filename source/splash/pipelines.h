#pragma once
#include "engine/vulkan/context.h"
#include "engine/vulkan/pipeline.h"
#include "engine/vulkan/target.h"

namespace kodanuki
{

// The graphics pipeline that renders fluid particles.
VulkanPipelineOld create_render_fluid_pipeline(VulkanContext device, VulkanTarget target);

}
