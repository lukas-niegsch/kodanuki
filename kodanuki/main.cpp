#include <iostream>
#include "engine/central/entity.h"
#include "engine/framework/game.h"
#include "engine/framework/module.h"
#include "engine/graphics/window.h"
using namespace Kodanuki;

class KodanukiRootModule : public Module
{
public:
	KodanukiRootModule(GameInfo& info)
	{
		modules.push_back(new WindowModule(info, {800, 600}));
	}

	virtual void onAttach() override
	{
		for (Module* module : modules)
		{
			module->onAttach();
		}
	}

	virtual void onDetach() override
	{
		for (Module* module : modules)
		{
			module->onDetach();
			delete module;
		}
	}

	virtual void onUpdate(float deltaTime) override
	{
		for (Module* module : modules)
		{
			module->onUpdate(deltaTime);
		}
	}

	virtual void onRender(float deltaTime) override
	{
		for (Module* module : modules)
		{
			module->onRender(deltaTime);
		}
	}

private:
	std::vector<Module*> modules;
};

int main()
{
	Entity game = ECS::create();

	ECS::update<GameInfo>(game,
	{
		.title = "Kodanuki",
		.updateHz = 30,
		.framesHz = 30,
		.running = false,
		.modules = nullptr 
	});

	GameInfo& info = ECS::get<GameInfo>(game);
	Module* root = new KodanukiRootModule(info);
	info.modules = root;
	int result = RunGame(game);
	delete root;
	return result;
}
