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
	auto window = std::make_shared<WindowModule>(info, extent);
	info.modules.push_back(window);
	info.modules.push_back(std::make_shared<RenderModule>(window));
	return RunGame(game);
}
