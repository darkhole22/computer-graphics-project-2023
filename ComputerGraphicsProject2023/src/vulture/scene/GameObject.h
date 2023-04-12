#pragma once

#include <glm/glm.hpp>

#include "vulture/renderer/Renderer.h"

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
		m_Uniform->model = glm::translate(m_Uniform->model, translation);
		m_Uniform.map();
	}

	friend class Scene;
private:
	Ref<Model> m_Model;
	Ref<Texture> m_Texture;
	Uniform<ModelBufferObject> m_Uniform;
};

} // vulture