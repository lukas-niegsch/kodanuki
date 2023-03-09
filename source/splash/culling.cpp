#include "source/splash/culling.h"
#include "source/splash/model.h"
#include <glm/glm.hpp>
#include <iostream>

namespace kodanuki
{

// Currently do these operations inside the CPU. It might be slower for
// now but we can use glm until all the tensor operations are implemented.
// see https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling

struct Plane
{
	glm::vec3 normal;
	float distance;
};

// Makes a plane from the general form ax + by + cz + d = 0.
Plane make_plane(float a, float b, float c, float d)
{
	Plane plane;
	plane.normal = {a, b, c};
	plane.distance = glm::abs(d) / glm::length(plane.normal);
	plane.normal = glm::normalize(plane.normal);
	return plane;
}

struct Frustum
{
	Plane face_top;
	Plane face_bot;
	Plane face_left;
	Plane face_right;
	Plane face_far;
	Plane face_near;
	Frustum(VulkanTensor mvp);
};

// http://web.archive.org/web/20120601083442/http://crazyjoke.free.fr/doc/3D/plane%20extraction.pdf
Frustum::Frustum(VulkanTensor mvp)
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	mvp.with_maps<MVP>([&](std::vector<MVP>& values) {
		model = 
		view = values[0].view;
		proj = values[0].projection;
	});

	float a, b, c, d;
	glm::mat4 combo = proj * view * model;

	a = combo[4 - 1][1 - 1] + combo[1 - 1][1 - 1];
	b = combo[4 - 1][2 - 1] + combo[1 - 1][2 - 1];
	c = combo[4 - 1][3 - 1] + combo[1 - 1][3 - 1];
	d = combo[4 - 1][4 - 1] + combo[1 - 1][4 - 1];
	this->face_left = make_plane(a, b, c, d);

	a = combo[4 - 1][1 - 1] - combo[1 - 1][1 - 1];
	b = combo[4 - 1][2 - 1] - combo[1 - 1][2 - 1];
	c = combo[4 - 1][3 - 1] - combo[1 - 1][3 - 1];
	d = combo[4 - 1][4 - 1] - combo[1 - 1][4 - 1];
	this->face_right = make_plane(a, b, c, d);

	a = combo[4 - 1][1 - 1] - combo[2 - 1][1 - 1];
	b = combo[4 - 1][2 - 1] - combo[2 - 1][2 - 1];
	c = combo[4 - 1][3 - 1] - combo[2 - 1][3 - 1];
	d = combo[4 - 1][4 - 1] - combo[2 - 1][4 - 1];
	this->face_top = make_plane(a, b, c, d);

	a = combo[4 - 1][1 - 1] + combo[2 - 1][1 - 1];
	b = combo[4 - 1][2 - 1] + combo[2 - 1][2 - 1];
	c = combo[4 - 1][3 - 1] + combo[2 - 1][3 - 1];
	d = combo[4 - 1][4 - 1] + combo[2 - 1][4 - 1];
	this->face_bot = make_plane(a, b, c, d);

	a = combo[4 - 1][1 - 1] + combo[3 - 1][1 - 1];
	b = combo[4 - 1][2 - 1] + combo[3 - 1][2 - 1];
	c = combo[4 - 1][3 - 1] + combo[3 - 1][3 - 1];
	d = combo[4 - 1][4 - 1] + combo[3 - 1][4 - 1];
	this->face_near = make_plane(a, b, c, d);

	a = combo[4 - 1][1 - 1] - combo[3 - 1][1 - 1];
	b = combo[4 - 1][2 - 1] - combo[3 - 1][2 - 1];
	c = combo[4 - 1][3 - 1] - combo[3 - 1][3 - 1];
	d = combo[4 - 1][4 - 1] - combo[3 - 1][4 - 1];
	this->face_far = make_plane(a, b, c, d);
}

bool is_sphere_before_plane(const Plane& plane, glm::vec3 position, float radius)
{
	float distance = glm::dot(plane.normal, position) - plane.distance;
	return distance > -radius;
}

bool is_sphere_inside_frustum(const Frustum& frustum, glm::vec3 position, float radius)
{
	bool inside = true;
	inside &= is_sphere_before_plane(frustum.face_top, position, radius);
	inside &= is_sphere_before_plane(frustum.face_bot, position, radius);
	inside &= is_sphere_before_plane(frustum.face_left, position, radius);
	inside &= is_sphere_before_plane(frustum.face_right, position, radius);
	inside &= is_sphere_before_plane(frustum.face_far, position, radius);
	inside &= is_sphere_before_plane(frustum.face_near, position, radius);
	return inside;
}

VulkanTensor cull_invisible_spheres(VulkanTensor spheres, VulkanTensor mvp, float p, float q)
{
	return spheres;
	Frustum frustum(mvp);

	std::vector<glm::vec3> visible_positions;
	spheres.with_maps<glm::vec3>([&](auto& values) {
		for (uint32_t i = 0; i < values.size(); i++) {
			if (is_sphere_inside_frustum(frustum, values[i], 1.0f)) {
				visible_positions.push_back(values[i]);
			}
		}
	});

	if (visible_positions.empty()) {
		float dummy = std::numeric_limits<float>::quiet_NaN();
		visible_positions.push_back({dummy, dummy, dummy});
	}

	std::cout << visible_positions.size() << '\n';

	auto builder = spheres.get_builder();
	builder.shape = {3, visible_positions.size()};
	VulkanTensor visibles(builder);

	visibles.with_maps<glm::vec3>([&](auto& values) {
		for (uint32_t i = 0; i < values.size(); i++) {
			values[i] = visible_positions[i];
		}
	});

	(void) p; (void) q;
	return visibles;
}

}