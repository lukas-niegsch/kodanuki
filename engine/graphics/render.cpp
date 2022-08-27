#include "engine/graphics/render.h"

namespace Kodanuki
{

RenderModule::RenderModule(std::shared_ptr<WindowModule> window)
{
	this->window = window;
}

void RenderModule::onAttach()
{

}

void RenderModule::onDetach()
{

}

}
