#pragma once

#include <glm/glm.hpp>

#include "vulture/renderer/Renderer.h"
#include "Scene.h"

namespace vulture{

struct ModelBufferObject
{
	glm::mat4 model = glm::mat4(1.0f);
};

class GameObject
{
public:
	explicit GameObject(const Renderer& renderer, const std::string& modelPath, const std::string& texturePath)
	{
		m_Model = renderer.makeBaseModel(modelPath);
		m_Uniform = renderer.makeUniform<ModelBufferObject>();
		m_Texture = renderer.makeTexture(texturePath);
	}

	void translate(glm::vec3 translation)
	{
		m_Position += translation;
	}

	inline glm::vec3 getPosition() { return m_Position; }

	inline glm::vec3 getScale() { return m_Scale; }
	inline void setScale(glm::vec3 scale) { m_Scale = scale; }

	friend class Scene;
private:
	Ref<Model> m_Model;
	Ref<Texture> m_Texture;
	Uniform<ModelBufferObject> m_Uniform;
	ObjectHandle handle = -1;

	glm::vec3 m_Position;
	glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);

	void update(float dt)
	{
		m_Uniform->model = glm::translate(glm::mat4(1.0f), m_Position) *
							glm::scale(glm::mat4(1.0f), m_Scale);
	}
};

} // vulture