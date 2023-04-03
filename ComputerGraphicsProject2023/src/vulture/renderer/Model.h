#pragma once

#include <functional>

#include <tiny_obj_loader.h>

#include "vulkan_wrapper.h"

namespace vulture {

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

	template<class _Vertex>
	Model(const Device& device, const std::string& modelPath,
		const std::function<_Vertex(const _BaseVertex&)> vertexBuilder)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::vector<_Vertex> vertices{};
		std::vector<uint32_t> indecies{};
		std::unordered_map<_BaseVertex, uint32_t, _BaseVertexHash> uniqueVertices{};

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

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertexBuilder(vertex));
				}

				indecies.push_back(uniqueVertices[vertex]);
			}
		}

		VkDeviceSize vertexBufferSize = sizeof(_Vertex) * vertices.size();
		Buffer vertexStagingBuffer(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vertexStagingBuffer.map(vertexBufferSize, vertices.data());
		m_VertexBuffer = Buffer(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vertexStagingBuffer.copyToBuffer(vertexBufferSize, m_VertexBuffer);

		VkDeviceSize indexBufferSize = sizeof(uint32_t) * indecies.size();
		Buffer indexStagingBuffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		indexStagingBuffer.map(indexBufferSize, indecies.data());
		m_IndexBuffer = Buffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		indexStagingBuffer.copyToBuffer(vertexBufferSize, m_IndexBuffer);
	}

private:
	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
};

} // namespace vulture 
