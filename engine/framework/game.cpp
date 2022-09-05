#include "engine/framework/game.h"
#include <algorithm>
#include <chrono>
#include <thread>
#include <iostream>
#include <cassert>

namespace Kodanuki
{

void RunGameLoop(GameInfo& game)
{
	using namespace std::chrono;
	using namespace std::this_thread;
	using Clock = steady_clock;
	using Time = time_point<Clock>;
	using Duration = nanoseconds;

	const int maxUpdates = 1 + game.updateHz / game.framesHz;
	const Duration updateDuration = Duration(1000000000 / game.updateHz);
	const Duration framesDuration = Duration(1000000000 / game.framesHz);
	Duration sleepDuration;

	Time lastUpdateTime = Clock::now();
	Time lastRenderTime = Clock::now();
	int updateCounter = 0;

	while (game.running)
	{
		const Time now = Clock::now();
		updateCounter = 0;

		while (now - lastUpdateTime > updateDuration && updateCounter < maxUpdates)
		{
			// todo: calculate updateDeltaTime
			// const float updateDeltaTime = 0.0;

			/*
			for (auto module : game.modules)
			{
				module->onUpdate(updateDeltaTime);
			}
			*/

			updateCounter++;
			lastUpdateTime += updateDuration;
		}

		//const duration<float> framesDeltaDuration = now - lastRenderTime;
		//float framesDeltaTime = 1000 * framesDeltaDuration.count();

		/*
		for (auto module : game.modules)
		{
			module->onRender(framesDeltaTime);
		}
		*/

		lastRenderTime = now;
		sleepDuration = std::max(framesDuration, framesDuration + lastUpdateTime - lastRenderTime);
		sleep_for(sleepDuration);
	}
}

int RunGame(Entity game)
{
	assert(ECS::has<GameInfo>(game));
	GameInfo& info = ECS::get<GameInfo>(game);
	info.running = true;

	/*
	for (auto module : info.modules)
	{
		module->onAttach();
	}
	*/

	RunGameLoop(info);

	/*
	for (auto module : info.modules)
	{
		module->onDetach();
	}
	*/

	return 0;
}
	
}
