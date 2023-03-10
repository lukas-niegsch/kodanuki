#pragma once
#include "source/splash/scene.h"
#include "source/splash/model.h"
#include "engine/vulkan/context.h"
#include "engine/vulkan/tensor.h"

namespace kodanuki
{

/**
 * The class that does SPH fluid simulation.
 *
 * We will be implementing some algorithms from the following paper:
 * https://cg.informatik.uni-freiburg.de/publications/2014_EG_SPH_STAR.pdf
 *
 * This will not be a complete framework, so we don't provide any way to
 * specify custom algorithms. This makes the implementation easier and is
 * currently all that we care about.
 *
 * The simulation holds vulkan tensors for the different properties of the
 * fluid system such as position and velocity.
 * 
 * We use enums to quickly switch between scenes and the different algorithms.
 */
class Simulation
{
public:
	/**
	 * Creates a new simulation for the given parameters.
	 * 
	 * This will load a default scene and algorithms at first which can be
	 * changed during runtime using the different setter methods.
	 * 
	 * @param device The device on which to execute the tensor operations.
	 */
	Simulation(VulkanContext device);

	/**
	 * Updates the tensors to load the new scene.
	 *
	 * @param scene The new scene to load.
	 */
	void load_scene(Scene scene);

	/**
	 * Updates some additional parameters that will be using inside the
	 * algorithms.
	 *
	 * Note that some parameters like the delta_time and particle_count
	 * will be ignored. These are just there for historical reasons.
	 *
	 * @param parameters The parameters that algorithms will use.
	 */
	void set_params(UD parameters);

	/**
	 * Executes the next step inside the fluid simulation.
	 *
	 * This will use all the parameters that have been specified using the
	 * different setter methods.
	 *
	 * @param delta_time The elapsed time between the last tick. 
	 */
	void tick_fluids(float delta_time);

public:
	/**
	 * Returns the mass tensor of the current frame.
	 * 
	 * @param frame The frame number module the frame count.
	 * @return The mass tensor of the current frame.
	 */
	VulkanTensor get_mass();

	/**
	 * Returns the position tensor of the current frame.
	 * 
	 * @return The position tensor of the current frame.
	 */
	VulkanTensor get_position();

	/**
	 * Returns the velocity tensor of the current frame.
	 * 
	 * @return The position tensor of the current frame.
	 */
	VulkanTensor get_velocity();

	/**
	 * Returns the pressure tensor of the current frame.
	 * 
	 * @return The pressure tensor of the current frame.
	 */
	VulkanTensor get_pressure();

	/**
	 * Returns the density tensor of the current frame.
	 * 
	 * @return The pressure tensor of the current frame.
	 */
	VulkanTensor get_density();

	/**
	 * Returns the number of particles in the current scene.
	 *
	 * @return The number of particels in the current scene.
	 */
	uint32_t get_particle_count();

private:
	/**
	 * Creates a new empty tensor for the simulation.
	 *
	 * @param shape The shape of the tensor.
	 * @return An empty tensor for the simulation.
	 */
	VulkanTensor create_tensor(std::vector<std::size_t> shape);

private:
	VulkanContext device;
	VulkanPipelineOldCache cache;
	uint32_t count_frame;
	uint32_t count_particles;
	UD parameters;
	VulkanTensor tensor_mass;
	VulkanTensor tensor_position;
	VulkanTensor tensor_velocity;
	VulkanTensor tensor_pressure;
	VulkanTensor tensor_density;
	bool update_descriptors;
};

}
