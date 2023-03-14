#pragma once
#include "source/splash/shader_bridge.h"
#include "source/splash/camera.h"
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
	VulkanDevice device;
	VulkanTarget target;
	VulkanWindow window;
	std::string fontfile;
	float fontsize;
};

class UserInterface
{
public:
	// Creates the user interface using imgui.
	UserInterface(UserInterfaceBuilder builder);

	// Deletes the user interface and frees the imgui context.
	~UserInterface();

	// Sleeps until the game loop should run again.
	bool tick();

	// Draws the interface to the given command buffer.
	void draw(VkCommandBuffer buffer);

	// Handles the camera movement based on the user input.
	void handle_input(ShaderBridge& bridge, uint32_t frame, float delta_time_seconds);

	// Shows the menu from the user interface.
	void show_menu(float delta_time_seconds);

private:
	VulkanDevice device;
	VulkanWindow window;
	VulkanTarget target;
	Camera camera;
	bool is_demo_open = false;
	bool is_menu_open = false;
	float render_distance = 150.0f;
	float move_speed = 60.0f;
	float look_speed = 90.0f;
	uint32_t max_dts_size = 60;
	std::vector<float> dts = std::vector<float>(max_dts_size, 0.0f);
};

}
