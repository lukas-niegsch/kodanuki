#include "engine/renderer/camera.h"
#include "engine/central/archetype.h"
#include "engine/central/entity.h"
#include "engine/defines/location.h"
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Kodanuki
{

void execute_camera_system()
{
	using System = Archetype<Require<Camera>, Iterate<Location>, Calculate<CameraView>>;
	for (auto[transform, view] : ECS->iterate<System>()) {
		glm::vec3 forward = glm::normalize(transform.direction);
		glm::vec3 right = glm::cross(forward, {0.0f, 1.0f, 0.0f});
		glm::vec3 up = glm::cross(forward, glm::normalize(right));

		view.view_matrix = glm::lookAt(
			transform.position,
			transform.position + transform.direction,
			up
		);
	}
}

}

