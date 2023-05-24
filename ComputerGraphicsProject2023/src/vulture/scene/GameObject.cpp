#include "GameObject.h"

namespace vulture {

ObjectHandle GameObject::s_NextHandle = 1; // 0 is the invalid handle.

GameObject::GameObject(const String &modelName, const String &textureName)
{
	m_Model = Model::get(modelName);
	m_Uniform = Renderer::makeUniform<vulture::ModelBufferObject>();
	m_Texture = Texture::get(textureName);
	m_TextureSampler = makeRef<TextureSampler>(*m_Texture);

	m_Handle = s_NextHandle;
	s_NextHandle++;
}

}