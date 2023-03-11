#pragma once
#include "engine/vulkan/device.h"
#include "engine/vulkan/target.h"
#include "engine/vulkan/window.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace kodanuki
{ 

struct UserInterfaceBuilder
{
	VulkanContext device;
	VulkanTarget target;
	VulkanWindow window;
	std::string fontfile;
	float fontsize;
};

class UserInterface
{
public:
	UserInterface(UserInterfaceBuilder builder);

	// Sleeps until the game loop should run again.
	bool tick();

	// Draws the interface to the given command buffer.
	void draw(VkCommandBuffer buffer);

private:
	// The abstract pointer to the implementation.
	std::shared_ptr<struct UserInterfaceState> state;
};


struct Config
{
	bool is_demo_open = false;
	bool is_menu_open = false;
	float render_distance = 150.0f;
	float move_speed = 60.0f;
	float look_speed = 90.0f;
	uint32_t max_dts_size = 60;
	std::vector<float> dts = std::vector<float>(max_dts_size, 0.0f);
};

void show_config(Config& config, float delta_time);

}
