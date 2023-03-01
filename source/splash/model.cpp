#include "source/splash/model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "extern/tinyobjloader/tiny_obj_loader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>

namespace
{

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

}

namespace std
{

template <>
struct hash<kodanuki::Vertex>
{
	size_t operator()(const kodanuki::Vertex& vertex) const
	{
		size_t seed = 0;
		hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
		return seed;
	}
};

}

namespace kodanuki
{

bool Vertex::operator==(const Vertex& other) const
{
	if (position != other.position) return false;
	if (color != other.color) return false;
	if (normal != other.normal) return false;
	if (uv != other.uv) return false;
	return true;
}

Model load_obj_model(std::string filename)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, error;
	(void) materials;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filename.c_str())) {
		throw std::runtime_error(warn + error);
		return {};
	}

	Model model = {};
	std::unordered_map<Vertex, uint32_t> unique_vertices;

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};
			vertex.color = {0.0f, 0.0f, 0.8f}; // default

			if (index.vertex_index >= 0) {
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
				};

				vertex.color = {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2],
				};
			}

			if (index.normal_index >= 0) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
				};
			}

			if (index.texcoord_index >= 0) {
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],
				};
			}

			if (unique_vertices.count(vertex) == 0) {
				unique_vertices[vertex] = static_cast<uint32_t>(model.vertices.size());
				model.vertices.push_back(vertex);
			}
			model.indices.push_back(unique_vertices[vertex]);
		}
	}

	return model;
}

}
