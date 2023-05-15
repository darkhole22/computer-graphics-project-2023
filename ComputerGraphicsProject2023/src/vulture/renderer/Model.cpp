#include "Model.h"

#include "Renderer.h"
#include "vulture/core/Logger.h"

#include <filesystem>

namespace vulture {

extern RendererData rendererData;

static bool loadModelFromObj(std::vector<Vertex>& vertices, std::vector<u32>& indecies, const String path);

Ref<Model> Model::get(const String& name)
{
	auto it = s_Models.find(name);
	if (it != s_Models.end())
	{
		auto& wref = it->second;
		if (!wref.expired())
			return wref.lock();
		else
			s_Models.erase(it);
	}

	Ref<Model> result;

	String namePrefix = rendererData.resourceInfo.path + "models/" + name;

	std::vector<Vertex> vertices{};
	std::vector<u32> indecies{};

	// if (std::filesystem::exists((namePrefix + ".gltf").cString()) && loadModelFromGltf(vertices, indecies))
	// {
	// 
	// } else
	if (std::filesystem::exists((namePrefix + ".obj").cString()) && loadModelFromObj(vertices, indecies, namePrefix))
	{
		result = Ref<Model>(new Model(vertices, indecies));
		s_Models.insert({ name, result });
	}
	else
	{
		VUERROR("Failed to load model for %s!", name.cString());
		result = s_Default;
	}

	return result;
}

static bool loadModelFromObj(std::vector<Vertex>& vertices, std::vector<u32>& indecies, const String path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	String fileName = path + ".obj";

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName.cString())) {
		VUERROR("Failed to load model at %s!", fileName.cString());
		return false;
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				},
				{
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				},
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2LL * index.texcoord_index + 1]
				}
			};

			// if (uniqueVertices.count(vertex) == 0) {
				//uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size()); }
			vertices.push_back(vertex);

			// indecies.push_back(uniqueVertices[vertex]);
			indecies.push_back(static_cast<u32>(vertices.size() - 1));
		}
	}
	// std::unordered_map<_BaseVertex, uint32_t, _BaseVertexHash> uniqueVertices{};
}

Model::Model(std::vector<Vertex> vertices, std::vector<u32> indecies) :
	m_IndexCount(static_cast<u32>(indecies.size()))
{
	VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();
	Buffer vertexStagingBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vertexStagingBuffer.map(vertices.data());
	m_VertexBuffer = Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vertexStagingBuffer.copyToBuffer(vertexBufferSize, m_VertexBuffer);

	VkDeviceSize indexBufferSize = sizeof(u32) * indecies.size();
	Buffer indexStagingBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	indexStagingBuffer.map(indecies.data());
	m_IndexBuffer = Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	indexStagingBuffer.copyToBuffer(indexBufferSize, m_IndexBuffer);
}

std::unordered_map<String, WRef<Model>> Model::s_Models = {};
Ref<Model> Model::s_Default;

void Model::makeDefaultModel()
{
	std::vector<Vertex> vertices{};
	std::vector<u32> indecies{};

	vertices.push_back({ {-1, 1, 0}, {0, 0, 1}, {0, 0} });
	vertices.push_back({ { 1, 1, 0}, {0, 0, 1}, {0, 1} });
	vertices.push_back({ {-1,-1, 0}, {0, 0, 1}, {1, 0} });
	vertices.push_back({ { 1,-1, 0}, {0, 0, 1}, {1, 1} });

	indecies.push_back(0);
	indecies.push_back(2);
	indecies.push_back(1);
	indecies.push_back(1);
	indecies.push_back(2);
	indecies.push_back(3);

	s_Default = Ref<Model>(new Model(vertices, indecies));
}

bool Model::init()
{
	makeDefaultModel();

	return true;
}

void Model::cleanup()
{
	s_Default.reset();
}

} // namespace vulture