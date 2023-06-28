#include "GameObject.h"

namespace vulture {

ObjectHandle GameObject::s_NextHandle = 1; // 0 is the invalid handle.

const String GameObject::c_DefaultEmissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME;
const String GameObject::c_DefaultRoughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME;

GameObject::GameObject(
		const String &modelName,
		const String &baseTextureName, const String &emissionTextureName, const String &roughnessTextureName,
		const glm::mat4& loadTransform
)
{
	m_Model = Model::get(modelName, loadTransform);
	m_ModelUniform = Renderer::makeUniform<ModelBufferObject>();

	m_BaseTexture = Texture::get("base/" + baseTextureName);
	m_TextureSampler = makeRef<TextureSampler>(*m_BaseTexture);

	m_EmissionTexture = Texture::get("emission/" + emissionTextureName);
	m_EmissionTextureSampler = makeRef<TextureSampler>(*m_EmissionTexture);

	m_RoughnessTexture = Texture::get("roughness/" + roughnessTextureName);
	m_RoughnessTextureSampler = makeRef<TextureSampler>(*m_RoughnessTexture);

	m_ObjectUniform = Renderer::makeUniform<ObjectBufferObject>();

	transform = makeRef<Transform>();

	m_Handle = s_NextHandle;
	s_NextHandle++;
}

}