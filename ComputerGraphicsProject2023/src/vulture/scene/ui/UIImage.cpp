#include "UIImage.h"

namespace vulture {

UIImage::UIImage(DescriptorPool& descriptorPool, Ref<DescriptorSetLayout> descriptorSetLayout,
				 const String& texture, glm::vec2 position, f32 scale) :
	m_TextureName(texture), m_DescriptorPool(&descriptorPool), m_DescriptorSetLayout(descriptorSetLayout)
{
	m_Uniform = Renderer::makeUniform<UIImageVertexBufferObject>();

	m_Uniform->position = position;
	m_Uniform->scale = scale;
}

void UIImage::setImage(const String& image)
{
	if (m_TextureName == image) return;

	m_TextureName = image;
	m_Modified = true;
}

void UIImage::setVisible(bool visible)
{
	m_Visible = visible;
	emit(UIImageRecreated{});
}

void UIImage::update(f32)
{
	if (m_Modified)
	{
		m_Texture = Texture::get(m_TextureName);
		m_TextureSampler = makeRef<TextureSampler>(*m_Texture);
		m_DescriptorSet = m_DescriptorPool->getDescriptorSet(m_DescriptorSetLayout, { *m_TextureSampler , m_Uniform });

		m_Uniform->ratio = static_cast<f32>(m_Texture->getWidth()) / m_Texture->getHeight();

		emit(UIImageRecreated{});
		m_Modified = false;
	}
}

} // namespace vulture
