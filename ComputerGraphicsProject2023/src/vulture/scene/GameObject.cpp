#include "GameObject.h"

namespace vulture {

ObjectHandle GameObject::s_NextHandle = 0;

GameObject::GameObject(const String &modelPath, const String &textureName)
{
	m_Model = Model::make(modelPath);
	m_Uniform = Renderer::makeUniform<vulture::ModelBufferObject>();
	m_Texture = Texture::get(textureName);
	m_TextureSampler = makeRef<TextureSampler>(*m_Texture);

	m_Handle = s_NextHandle;
	s_NextHandle++;
}

void GameObject::update(f64 dt)
{
	m_Uniform->model =
		glm::translate(glm::mat4(1.0f), m_Position) *
		glm::mat4(m_Rotation) *
		glm::scale(glm::mat4(1.0f), m_Scale);
}

}