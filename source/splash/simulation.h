#pragma once
#include "source/splash/scene.h"
#include "source/splash/model.h"
#include "engine/vulkan/device.h"
#include "engine/vulkan/tensor.h"
#include <vector>

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
 * fluid system such as position and velocity. Because the rendering uses
 * frames in flight we actually store multiple copies simultaneously. The
 * rendering should only use the calculated positions for the current frame.
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
	 * @param frame_count The number of simultaneous tensor copies.
	 */
	Simulation(VulkanDevice device, uint32_t frame_count);

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
	 * @param frame The frame number module the frame count.
	 * @param delta_time The elapsed time between the last tick. 
	 */
	void tick_fluids(uint32_t frame, float delta_time);

public:
	/**
	 * Returns the mass tensor of the current frame.
	 * 
	 * @param frame The frame number module the frame count.
	 * @return The mass tensor of the current frame.
	 */
	VulkanTensor get_mass(uint32_t frame);

	/**
	 * Returns the position tensor of the current frame.
	 * 
	 * @param frame The frame number module the frame count.
	 * @return The position tensor of the current frame.
	 */
	VulkanTensor get_position(uint32_t frame);

	/**
	 * Returns the velocity tensor of the current frame.
	 * 
	 * @param frame The frame number module the frame count.
	 * @return The position tensor of the current frame.
	 */
	VulkanTensor get_velocity(uint32_t frame);

	/**
	 * Returns the pressure tensor of the current frame.
	 * 
	 * @param frame The frame number module the frame count.
	 * @return The pressure tensor of the current frame.
	 */
	VulkanTensor get_pressure(uint32_t frame);

	/**
	 * Returns the density tensor of the current frame.
	 * 
	 * @param frame The frame number module the frame count.
	 * @return The pressure tensor of the current frame.
	 */
	VulkanTensor get_density(uint32_t frame);

private:
	/**
	 * Deletes all the tensor data and sets the particle count to zero.
	 */
	void reset_tensors();

	/**
	 * Creates a new empty tensor for the simulation.
	 *
	 * @param shape The shape of the tensor.
	 * @return An empty tensor for the simulation.
	 */
	VulkanTensor create_tensor(std::vector<std::size_t> shape);

private:
	VulkanDevice device;
	VulkanPipelineCache cache;
	uint32_t count_frame;
	uint32_t count_particles;
	UD parameters;
	std::vector<VulkanTensor> tensors_mass;
	std::vector<VulkanTensor> tensors_position;
	std::vector<VulkanTensor> tensors_velocity;
	std::vector<VulkanTensor> tensors_pressure;
	std::vector<VulkanTensor> tensors_density;
};

}
