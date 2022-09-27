#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Kodanuki
{

// Signals that the entity has a camera.
struct Camera {};

/**
 * The camera view contains the calculated camera view.
 */
struct CameraView
{
	// The view matrix for the camera [world space -> camera space].
	glm::mat4 view_matrix;
};


/**
 * Iterates over all entities wil the camera component
 * and updates the camera view. The entity is required
 * to have a transform component.
 * 
 * This will skip over entities which already has some
 * camera view. The view must be removed to recalculate
 * it.
 */
void execute_camera_system();

}

