#include "source/splash/simulation.h"

namespace kodanuki
{

Simulation::Simulation(VulkanDevice device)
: device(device)
, tensor_mass(create_tensor({1}))
, tensor_position(create_tensor({1}))
, tensor_velocity(create_tensor({1}))
, tensor_pressure(create_tensor({1}))
, tensor_density(create_tensor({1}))
{
	parameters.delta_time = 0.0f;
	parameters.kernel_size = 1.2;
	parameters.stiffness = 0.0;
	parameters.rho_0 = 1.0;
	parameters.gravity = 10;
	parameters.viscosity = 0.2;
	parameters.particle_count = 0;
}

void Simulation::load_scene(Scene scene)
{
	count_particles = scene.instance_count;
	tensor_mass = create_tensor({1, count_particles});
	tensor_position = create_tensor({3, count_particles});
	tensor_velocity = create_tensor({3, count_particles});
	tensor_pressure = create_tensor({1, count_particles});
	tensor_density = create_tensor({1, count_particles});
	tensor_mass.load_data(scene.masses);
	tensor_position.load_data(scene.positions);
	tensor_velocity.load_data(scene.velocities);
	update_descriptors = true;
}

void Simulation::set_params(UD parameters)
{
	this->parameters = parameters;
}

void Simulation::tick_fluids(float delta_time)
{
	// TODO: implement this method properly
	vt::linear_i(1.0f, tensor_position, delta_time, tensor_velocity, update_descriptors);
	update_descriptors = false;
}

VulkanTensor Simulation::get_mass()
{
	return tensor_mass;
}

VulkanTensor Simulation::get_position()
{
	return tensor_position;
}

VulkanTensor Simulation::get_velocity()
{
	return tensor_velocity;
}

VulkanTensor Simulation::get_pressure()
{
	return tensor_pressure;
}

VulkanTensor Simulation::get_density()
{
	return tensor_density;
}

uint32_t Simulation::get_particle_count()
{
	return count_particles;
}

VulkanTensor Simulation::create_tensor(std::vector<std::size_t> shape)
{
	VulkanTensor tensor = {{
		.device = device,
		.cache = cache,
		.shape = shape,
		.dtype = vt::eFloat,
		.dshare = vt::eUnique,
		.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
	}};
	return tensor;
}

}
