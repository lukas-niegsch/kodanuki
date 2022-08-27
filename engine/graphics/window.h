#pragma once
#include "engine/framework/game.h"
#include "engine/framework/module.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Kodanuki
{

/**
 * This module creates and manages an GLFW window.
 * 
 * It also provides some utility functions for interacting
 * with the underlying GLFW window object.
 */
class WindowModule : public Module
{
public:
	// Creates a new window with the given size.
	WindowModule(GameInfo& game, VkExtent2D extend);

	// Called once when the module is attached.
	virtual void onAttach() override;

	// Called once when the module is detached.
	virtual void onDetach() override;

	// Called repeatedly to update the module.
	virtual void onUpdate(float deltaTime) override;

	// Creates an surface for the renderer to use.
	VkSurfaceKHR createSurface(VkInstance instance);

	// Resets the current extend of the window.
	void resetExtend();

	// Changes the current extend of the window.
	void setExtend(VkExtent2D extend);

	// Returns the current extend of the window.
	VkExtent2D getExtend();

private:
	GameInfo& info;
	GLFWwindow* window;
	VkExtent2D defaultExtend;
};

}
