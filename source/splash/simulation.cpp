#include "source/splash/simulation.h"

namespace kodanuki
{

Simulation::Simulation(VulkanDevice device, uint32_t frame_count) : device(device)
{
	this->count_frame = frame_count;
	parameters.delta_time = 0.0f;
	parameters.kernel_size = 1.2;
	parameters.stiffness = 0.0;
	parameters.rho_0 = 1.0;
	parameters.gravity = 10;
	parameters.viscosity = 0.2;
	parameters.particle_count = 0;
	reset_tensors();
}

void Simulation::load_scene(Scene scene)
{
	reset_tensors();
	count_particles = scene.instance_count;

	for (uint32_t frame = 0; frame < count_frame; frame++) {
		tensors_mass.push_back(create_tensor({1, count_particles}));
		tensors_position.push_back(create_tensor({3, count_particles}));
		tensors_velocity.push_back(create_tensor({3, count_particles}));
		tensors_pressure.push_back(create_tensor({1, count_particles}));
		tensors_density.push_back(create_tensor({1, count_particles}));
		tensors_mass[frame].load_data(scene.masses);
		tensors_position[frame].load_data(scene.positions);
		tensors_velocity[frame].load_data(scene.velocities);
	}
}

void Simulation::set_params(UD parameters)
{
	this->parameters = parameters;
}

void Simulation::tick_fluids(uint32_t frame, float delta_time)
{
	// TODO: implement this method properly
	tensors_position[frame] = get_position(frame - 1) + delta_time * get_velocity(frame - 1);
}

VulkanTensor Simulation::get_mass(uint32_t frame)
{
	return tensors_mass[frame % count_frame];
}

VulkanTensor Simulation::get_position(uint32_t frame)
{
	return tensors_position[frame % count_frame];
}

VulkanTensor Simulation::get_velocity(uint32_t frame)
{
	return tensors_velocity[frame % count_frame];
}

VulkanTensor Simulation::get_pressure(uint32_t frame)
{
	return tensors_pressure[frame % count_frame];
}

VulkanTensor Simulation::get_density(uint32_t frame)
{
	return tensors_density[frame % count_frame];
}

void Simulation::reset_tensors()
{
	tensors_mass.clear();
	tensors_position.clear();
	tensors_velocity.clear();
	tensors_pressure.clear();
	tensors_density.clear();
	count_particles = 0;
}

VulkanTensor Simulation::create_tensor(std::vector<std::size_t> shape)
{
	VulkanTensor tensor = {{
		.device = device,
		.cache = cache,
		.shape = shape,
		.dtype = vt::eFloat,
		.dshare = vt::eUnique
	}};
	return tensor;
}

}
