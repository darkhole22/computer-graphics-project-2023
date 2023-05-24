#pragma once

#include <functional>

#include <tiny_obj_loader.h>

#include "Buffers.h"

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
	static Ref<Model> get(const String& name);

	static Ref<Model> getPlane(u32 hCount = 1, u32 vCount = 1);

	inline const Buffer& getVertexBuffer() const { return m_VertexBuffer; }
	inline const Buffer& getIndexBuffer() const { return m_IndexBuffer; }
	inline u32 getIndexCount() const { return m_IndexCount; }

	friend class Renderer;
private:
	Model(std::vector<Vertex> vertices, std::vector<u32> indices);

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	u32 m_IndexCount = 0;

	static bool init();
	static void cleanup();

	static std::unordered_map<String, WRef<Model>> s_Models;
	static Ref<Model> s_Default;

	static void makeDefaultModel();
};

} // namespace vulture
