#include "engine/concept/polygon.h"

namespace kodanuki
{

constexpr Polygon GetUnitCube()
{
	std::vector<glm::vec3> vertices;
	vertices.push_back(glm::vec3(-1, -1, -1));
	vertices.push_back(glm::vec3(-1, -1, +1));
	vertices.push_back(glm::vec3(-1, +1, -1));
	vertices.push_back(glm::vec3(-1, +1, +1));
	vertices.push_back(glm::vec3(+1, -1, -1));
	vertices.push_back(glm::vec3(+1, -1, +1));
	vertices.push_back(glm::vec3(+1, +1, -1));
	vertices.push_back(glm::vec3(+1, +1, +1));
	return Polygon {vertices};
}

constexpr Polygon GetUnitGeodesicPolyhedron(int q, int b, int c)
{
	// TODO: implement this function
	(void) q; (void) b; (void) c;
	std::vector<glm::vec3> vertices;
	return Polygon {vertices};
}

};
