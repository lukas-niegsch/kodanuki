#include "engine/framework/module.h"

namespace Kodanuki
{
    
void Module::onAttach()
{
	// I am called once when the application starts.
}

void Module::onDetach()
{
	// I am called once when the application stops.
}

void Module::onUpdate(float)
{
	// I am called multiple times per second based on GameInfo::updateHz.
}

void Module::onRender(float)
{
	// I am called multiple times per second based on GameInfo::renderHz.
}

}
