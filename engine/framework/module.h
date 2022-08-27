#pragma once

namespace Kodanuki
{

/**
 * The component that represents an entire module. The
 * application will call the corresponding functions.
 */
class Module
{
public:
	// Virtual Destructor to avoid calling the wrong destructor.
	virtual ~Module() = default;

	// Called once when the module is attached.
	virtual void onAttach();

	// Called once when the module is detached.
	virtual void onDetach();

	// Called repeatedly to update the module.
	virtual void onUpdate(float deltaTime);

	// Called repeatedly to render the module.
	virtual void onRender(float deltaTime);
};

}
