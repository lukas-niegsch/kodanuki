#pragma once
#include "engine/graphics/window.h"
#include "engine/framework/module.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Kodanuki
{

/**
 * This module creates and manages a vulkan renderer.
 * 
 * Currently all components are combined in this class
 * for convinience since no architecture is chosen yet.
 */
class RenderModule : public Module
{
public:
	// Constructs the new render module.
	RenderModule(std::shared_ptr<WindowModule> window);

	// Called once when the module is attached.
	virtual void onAttach() override;

	// Called once when the module is detached.
	virtual void onDetach() override;

private:
	std::shared_ptr<WindowModule> window;
};

}
