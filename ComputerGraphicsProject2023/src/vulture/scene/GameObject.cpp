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

}