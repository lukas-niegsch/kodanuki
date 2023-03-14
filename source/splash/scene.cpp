#include "source/splash/scene.h"
#include <random>

namespace kodanuki
{

Scene load_csv_scene(std::string filename)
{
	(void) filename;
	// TODO: implement properly ...

	Scene scene;
	scene.instance_count = 4096; // 81, 1024, 9000, 30000, 65536, 1000000;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<float> dist(0.0f, std::cbrt(scene.instance_count / 2));

	for (uint32_t i = 0; i < scene.instance_count; i++) {
		scene.positions.push_back({dist(gen), dist(gen), dist(gen)});
		scene.velocities.push_back({5, 0, 0});
		scene.masses.push_back(0.0);
	}

	return scene;
}

}
