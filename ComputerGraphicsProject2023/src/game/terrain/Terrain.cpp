#include "Terrain.h"

namespace game {

using namespace vulture;

Terrain::Terrain()
{
	m_Scene = Application::getScene();

	m_DescriptorSetLayout = makeRef<DescriptorSetLayout>();
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->create();
	m_Pipeline = m_Scene->makePipeline("res/shaders/Terrain_vert.spv", "res/shaders/Terrain_frag.spv", m_DescriptorSetLayout);

	m_NoiseTexture = Texture::getNoise(512, 512, 15, {});

	TextureSamplerConfig samplerConfig;
	samplerConfig.setAddressMode(VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
	m_NoiseSampler = makeRef<TextureSampler>(*m_NoiseTexture, samplerConfig);

	m_Uniform = Renderer::makeUniform<ModelBufferObject>();
	
	m_Uniform->model = glm::translate(glm::mat4(1), { -5, -20, -15 }) *
		glm::scale(glm::mat4(1), { 100, 1, 100 });

	m_DescriptorSet = m_Scene->getDescriptorPool()->getDescriptorSet(*m_DescriptorSetLayout, { m_Uniform, *m_NoiseSampler, *m_NoiseSampler });
	m_Model = Model::getPlane(500, 500);
	m_Object = m_Scene->addObject(m_Pipeline, m_Model, m_DescriptorSet);
}

void Terrain::update(float dt)
{

}

} // namespace game
