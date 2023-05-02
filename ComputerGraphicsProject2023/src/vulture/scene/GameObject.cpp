#include "GameObject.h"

namespace vulture {

ObjectHandle GameObject::s_NextHandle = 0;

GameObject::GameObject(const std::string &modelPath, const std::string &texturePath)
{
	m_Model = Ref<Model>(Model::make(modelPath));
	m_Uniform = Renderer::makeUniform<vulture::ModelBufferObject>();
	m_Texture = Ref<Texture>(new Texture(texturePath));
	m_TextureSampler = makeRef<TextureSampler>(*m_Texture);

	m_Handle = s_NextHandle;
	s_NextHandle++;
}

void GameObject::update(float dt)
{
	// m_Uniform->model = glm::translate(glm::mat4(1.0f), m_Position) * glm::mat4(m_Rotation) * glm::scale(glm::mat4(1.0f), m_Scale);
	auto r = glm::mat4(m_Rotation);
	m_Uniform->model = glm::translate(glm::mat4(1.0f), m_Position) * r * glm::scale(glm::mat4(1.0f), m_Scale); // *;
}

}