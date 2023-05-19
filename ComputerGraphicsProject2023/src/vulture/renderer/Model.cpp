#include "Model.h"

#include "Renderer.h"
#define VU_LOGGER_DISABLE_INFO
#include "vulture/core/Logger.h"

#include <filesystem>

namespace vulture {

class VertexHash
{
public:
	u64 operator()(const Vertex& v) const
	{
		auto hash = std::hash<float>();
		return hash(v.pos.x) ^ hash(v.pos.y) ^ hash(v.pos.z) ^
			hash(v.norm.x) ^ hash(v.norm.y) ^ hash(v.norm.z) ^
			hash(v.texCoord.x) ^ hash(v.texCoord.y);
	}
};

extern RendererData rendererData;

static bool loadModelFromObj(std::vector<Vertex>& vertices, std::vector<u32>& indices, const String& path);

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
	std::vector<u32> indices{};

	// if (std::filesystem::exists((namePrefix + ".gltf").cString()) && loadModelFromGltf(vertices, indices))
	// {
	// 
	// } else
	if (std::filesystem::exists((namePrefix + ".obj").cString()) && loadModelFromObj(vertices, indices, namePrefix))
	{
		result = Ref<Model>(new Model(vertices, indices));
		s_Models.insert({ name, result });
	}
	else
	{
		VUERROR("Failed to load model for %s!", name.cString());
		result = s_Default;
	}

	return result;
}

static bool loadModelFromObj(std::vector<Vertex>& vertices, std::vector<u32>& indices, const String& path)
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

	std::unordered_map <Vertex, u32, VertexHash, decltype([](const Vertex& v1, const Vertex& v2) -> bool {
		return v1.pos == v2.pos && v1.norm == v2.norm && v1.texCoord == v2.texCoord;
	})> uniqueVertices{};

#ifdef VU_LOGGER_INFO_ENABLED
	u32 totalVertexCount = 0;
#endif
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

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<u32>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
#ifdef VU_LOGGER_INFO_ENABLED
			++totalVertexCount;
#endif
		}
	}
	VUINFO("Model loaded at %s. Total vertex: %d. Vertex used: %d.", fileName.cString(), totalVertexCount, vertices.size());
	return true;
}

Model::Model(std::vector<Vertex> vertices, std::vector<u32> indices) :
	m_IndexCount(static_cast<u32>(indices.size()))
{
	VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();
	Buffer vertexStagingBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vertexStagingBuffer.map(vertices.data());
	m_VertexBuffer = Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vertexStagingBuffer.copyToBuffer(vertexBufferSize, m_VertexBuffer);

	VkDeviceSize indexBufferSize = sizeof(u32) * indices.size();
	Buffer indexStagingBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	indexStagingBuffer.map(indices.data());
	m_IndexBuffer = Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	indexStagingBuffer.copyToBuffer(indexBufferSize, m_IndexBuffer);
}

std::unordered_map<String, WRef<Model>> Model::s_Models = {};
Ref<Model> Model::s_Default;

void Model::makeDefaultModel()
{
	std::vector<Vertex> vertices{};
	std::vector<u32> indices{};

	vertices.push_back({ {-1, 1, 0}, {0, 0, 1}, {0, 0} });
	vertices.push_back({ { 1, 1, 0}, {0, 0, 1}, {0, 1} });
	vertices.push_back({ {-1,-1, 0}, {0, 0, 1}, {1, 0} });
	vertices.push_back({ { 1,-1, 0}, {0, 0, 1}, {1, 1} });

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	s_Default = Ref<Model>(new Model(vertices, indices));
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