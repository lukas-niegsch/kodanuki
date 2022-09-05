#pragma once
#include "engine/central/family.h"

namespace Kodanuki
{

/**
 * The component that represents an module.
 * 
 * The methods are called once the module changes its context inside
 * the ECS. Changing the context is done when the module is added or
 * removed from the ECS.
 */
class Module
{
public:
	// Virtual Destructor to avoid calling the wrong destructor.
	virtual ~Module() = default;

	// Called when the module is attached to the ECS.
	virtual void attach(Family context);

	// Called when the module changes its family.
	virtual void update(Family context);

	// Called when the module is detached from the ECS.
	virtual void detach(Family context);
};

}
