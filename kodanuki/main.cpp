#include <iostream>
#include "engine/central/entity.h"
#include "engine/framework/game.h"
#include "engine/framework/module.h"
#include "engine/graphics/window.h"
#include "engine/graphics/render.h"
using namespace Kodanuki;

int main()
{
	Entity game = ECS::create();

	ECS::update<GameInfo>(game,
	{
		.title = "Kodanuki",
		.updateHz = 30,
		.framesHz = 30,
		.running = false,
		.modules = {} 
	});

	VkExtent2D extent = {800, 700};
	GameInfo& info = ECS::get<GameInfo>(game);
	auto windowModule = std::make_shared<WindowModule>(info, extent);
	auto renderModule = std::make_shared<RenderModule>(info, windowModule); 
	info.modules.push_back(windowModule);
	info.modules.push_back(renderModule);
	return RunGame(game);
}
