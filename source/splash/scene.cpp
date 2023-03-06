#include "source/splash/scene.h"

namespace kodanuki
{

Scene load_csv_scene(std::string filename)
{
	(void) filename;
	// TODO: implement properly ...

	Scene scene;
	scene.instance_count = 65536;

	for (uint32_t i = 0; i < scene.instance_count; i++) {
		scene.positions.push_back({10 * (i % 9), 0, 10 * (i / 9)});
		scene.velocities.push_back({10, 0, 0});
		scene.masses.push_back(0.0);
	}

	return scene;
}

}
