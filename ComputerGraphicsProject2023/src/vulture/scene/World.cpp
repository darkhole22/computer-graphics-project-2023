#include "World.h"

namespace vulture {

World::World(DescriptorPool& descriptorsPool)
	: m_Uniform(Renderer::makeUniform<WorldBufferObject>()), m_DescriptorSetLayout(makeRef<DescriptorSetLayout>())
{
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->create();

	m_DescriptorSet = descriptorsPool.getDescriptorSet(m_DescriptorSetLayout, {m_Uniform});
}

void World::updateUniforms(FrameContext& target, const Camera& camera)
{
	auto [index, count] = target.getFrameInfo();

	if (m_DescriptorSet)
	{
		m_Uniform->cameraPosition = camera.position;
		m_Uniform->lightColor = directLight.color;
		m_Uniform->lightDirection = directLight.direction;
		m_DescriptorSet->map(index);
	}
}

} // vulture
