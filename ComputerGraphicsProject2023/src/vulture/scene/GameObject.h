#pragma once

#include <glm/glm.hpp>
#include <string>
#include "vulture/renderer/Model.h"
#include "vulture/renderer/Renderer.h"

namespace vulture{

using ObjectHandle = int64_t;

struct ModelBufferObject
{
	glm::mat4 model = glm::mat4(1.0f);
};

class GameObject
{
public:
	explicit GameObject(const std::string& modelPath, const std::string& texturePath)
	{
		m_Model = Ref<Model>(Model::make(modelPath));
		m_Uniform = Renderer::makeUniform<vulture::ModelBufferObject>();
		m_Texture = Ref<Texture>(new Texture(texturePath));
		m_TextureSampler = makeRef<TextureSampler>(*m_Texture);

		m_Handle = s_NextHandle;
		s_NextHandle++;
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
	Ref<TextureSampler> m_TextureSampler;
	Uniform<ModelBufferObject> m_Uniform;

	ObjectHandle m_Handle = -1;
	static ObjectHandle s_NextHandle;

	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);

	void update(float dt)
	{
		m_Uniform->model = glm::translate(glm::mat4(1.0f), m_Position) *
							glm::scale(glm::mat4(1.0f), m_Scale);
	}
};

} // vulture