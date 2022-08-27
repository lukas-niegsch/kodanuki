#pragma once
#include "engine/framework/module.h"
#include "engine/central/entity.h"
#include <vector>

namespace Kodanuki
{

/**
 * Holds all the information for the current game. Except for the name, all
 * other values can be changed during execution. Currently only one game
 * should be active at once.
 */
struct GameInfo
{
	// The title of the game.
	const char* title;

	// The amount of updates that are called each second.
	int updateHz;

	// The amount of frames that are rendered each second.
	int framesHz;

	// Is the game currently running?
	bool running;

	// The main module that the game should run.
	Module* modules;
};

/**
 * Starts the game and executes it until the running flag inside the game
 * info is set to false. The game is implemented inside the different modules.
 * 
 * @param game The entity that contains the game info.
 * @return The result of running the game.
 */
int RunGame(Entity game);

}
