#pragma once
#include "engine/vulkan/device.h"

/**
 * Creates the default device that can be used for testing. This device is not
 * suitable for rendering. This will select the first available graphics card.
 */
kodanuki::VulkanDevice default_device();
