#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include "vulture/renderer/Model.h"
#include "vulture/renderer/Renderer.h"
#include "vulture/core/Logger.h"


namespace vulture{

using ObjectHandle = int64_t;

struct ModelBufferObject
{
	glm::mat4 model = glm::mat4(1.0f);
};

class GameObject
{
public:
	explicit GameObject(const std::string& modelPath, const std::string& texturePath);

	inline glm::vec3 getPosition() { return m_Position; }
	inline void setPosition(glm::vec3 pos) { m_Position = pos; }
	inline void setPosition(float x, float y, float z) { setPosition(glm::vec3(x, y, z)); }

	inline void translate(glm::vec3 translation) { m_Position += translation; }
	inline void translate(float x, float y, float z) { translate(glm::vec3(x, y, z)); }

	inline glm::vec3 getScale() { return m_Scale; }
	inline void setScale(glm::vec3 scale) { m_Scale = scale; }
	inline void setScale(float x, float y, float z) { setScale(glm::vec3(x, y, z)); }

	inline glm::quat getRotation() { return m_Rotation; }
	inline void setRotation(glm::vec3 rot) { m_Rotation = glm::quat(rot); }
	inline void setRotation(float x, float y, float z) { setRotation(glm::vec3(x, y, z)); }

	friend class Scene;
private:
	Ref<Model> m_Model;
	Ref<Texture> m_Texture;
	Ref<TextureSampler> m_TextureSampler;
	Uniform<ModelBufferObject> m_Uniform;

	ObjectHandle m_Handle = -1;
	static ObjectHandle s_NextHandle;

	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::quat m_Rotation = glm::quat(glm::vec3(0.0f));

	void update(float dt)
	{
		m_Uniform->model = glm::translate(glm::mat4(1.0f), m_Position) *
							glm::mat4(m_Rotation) *
							glm::scale(glm::mat4(1.0f), m_Scale);
	}
};

} // vulture