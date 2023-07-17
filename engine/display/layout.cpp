#include "engine/display/layout.h"
#include "engine/central/family.h"

namespace kodanuki
{

LayoutSystem::operator Entity()
{
	return itself;
}

LayoutSystem LayoutSystem::create(Builder builder)
{
	(void) builder;
	return {};
}

void render_roots()
{

}

void LayoutSystem::tick()
{
	render_roots();
}

}
