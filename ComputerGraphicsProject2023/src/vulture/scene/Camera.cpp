#include "Camera.h"

namespace vulture {

Camera::Camera(const Renderer& renderer, DescriptorPool& descriptorsPool) :
	m_Uniform(renderer.makeUniform<CameraBufferObject>()), m_DescriptorSetLayout(renderer.makeDescriptorSetLayout())
{
	m_DescriptorSetLayout.addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout.create();

	m_DescriptorSet = descriptorsPool.getDescriptorSet(m_DescriptorSetLayout, {	m_Uniform });
}

} // namespace vulture
