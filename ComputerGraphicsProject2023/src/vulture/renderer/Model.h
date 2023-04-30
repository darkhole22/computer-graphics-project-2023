#pragma once

#include <functional>

#include <tiny_obj_loader.h>

#include "vulkan_wrapper.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace vulture {

struct Vertex
{
	glm::vec3 pos = { 0, 0, 0 };
	glm::vec3 norm = { 0, 0 , 0 };
	glm::vec2 texCoord = { 0, 0 };
};

class Model
{
public:
	struct _BaseVertex
	{
		// Position
		tinyobj::real_t x;
		tinyobj::real_t y;
		tinyobj::real_t z;

		// Normals
		tinyobj::real_t nx;
		tinyobj::real_t ny;
		tinyobj::real_t nz;

		// Texture coordinates
		tinyobj::real_t u;
		tinyobj::real_t v;

		bool operator==(const _BaseVertex o)
		{
#define E(m) (o. m == m)
			return E(x) && E(y) && E(z) && E(nx) && E(ny) && E(nz) && E(u) && E(v);
#undef E
		}
	};

	struct _BaseVertexHash
	{
		size_t operator()(_BaseVertex vertex) const
		{
#define HASH(field, s) (std::hash<tinyobj::real_t>()(vertex. field) << s)
			return HASH(x, 0) ^ HASH(y, 1) ^ HASH(z, 2) ^
				HASH(nx, 3) ^ HASH(ny, 4) ^ HASH(nz, 5) ^
				HASH(u, 6) ^ HASH(v, 7);
#undef HASH
		}
	};

	struct VertexBuilder
	{
		Vertex operator()(const _BaseVertex& vertex)
		{
			Vertex v = {
				{vertex.x, vertex.y, vertex.z},
				{vertex.nx, vertex.ny, vertex.nz},
				{vertex.u, vertex.v}
			};

			return v;
		}
	};

	// template<class _Vertex, class _VertexBuilder>
	static Model* make(const Device& device, const String& modelPath)
	{
		Model* m = new Model();

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.cString())) {
			throw std::runtime_error(warn + err);
		}

		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indecies{};
		// std::unordered_map<_BaseVertex, uint32_t, _BaseVertexHash> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				_BaseVertex vertex{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2LL * index.texcoord_index + 1]
				};

				// if (uniqueVertices.count(vertex) == 0) {
					//uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size()); }
				vertices.push_back(VertexBuilder()(vertex));

				// indecies.push_back(uniqueVertices[vertex]);
				indecies.push_back(static_cast<uint32_t>(vertices.size()-1));
			}
		}

		VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();
		Buffer vertexStagingBuffer(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vertexStagingBuffer.map(vertices.data());
		m->m_VertexBuffer = Buffer(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vertexStagingBuffer.copyToBuffer(vertexBufferSize, m->m_VertexBuffer);

		VkDeviceSize indexBufferSize = sizeof(uint32_t) * indecies.size();
		Buffer indexStagingBuffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		indexStagingBuffer.map(indecies.data());
		m->m_IndexBuffer = Buffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		indexStagingBuffer.copyToBuffer(indexBufferSize, m->m_IndexBuffer);

		m->m_IndexCount = static_cast<uint32_t>(indecies.size());

		return m;
	}

	inline const Buffer& getVertexBuffer() const { return m_VertexBuffer; }
	inline const Buffer& getIndexBuffer() const { return m_IndexBuffer; }
	inline uint32_t getIndexCount() const { return m_IndexCount; }
private:
	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	uint32_t m_IndexCount = 0;
};

} // namespace vulture
