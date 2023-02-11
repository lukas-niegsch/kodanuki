#pragma once
#include <glm/vec3.hpp>
#include <vector>

namespace kodanuki
{

/**
 * Each polygon contains a 3D vertices.
 */
struct Polygon
{
	// The vertices of the unit polygon inside the unit cube.
	std::vector<glm::vec3> vertices;
};

/**
 * Get the unit cube.
 * 
 * @return const Polygon The resulting unit cube.
 */
constexpr Polygon GetUnitCube();

/**
 * Get the unit Geodesic Polyhedron from the geodesic notation.
 * 
 * Example:
 * q = 3, b = 1, c = 0 => Tetrahedron
 * q = 4, b = 1, c = 0 => Octahedron
 * q = 5, b = 1, c = 0 => Icosahedron
 * 
 * @param q The number of vertices for each base face.
 * @param b The first parameter of the subdevision description.
 * @param c The second parameter of the subdevision description.
 * @return constexpr Polygon The resulting unit polygon.
 */
constexpr Polygon GetUnitGeodesicPolyhedron(int q, int b, int c);

}

#include "engine/concept/polygon.tpp"
