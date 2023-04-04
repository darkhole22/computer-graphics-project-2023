#include "Scene.h"

#include <iostream>

namespace vulture {

RenderableObject::RenderableObject(const Pipeline& pipeline, const std::filesystem::path& path)
{

}

SceneObjectList::SceneObjectList(const Renderer& renderer, const std::string& vertexShader, 
	const std::string& fragmentShader, const std::vector<DescriptorSetLayout*>& descriptorSetLayouts) :
	m_Pipeline(new Pipeline(renderer.getRenderPass(), vertexShader, fragmentShader, descriptorSetLayouts, Renderer::getVertexLayout()))
{
}

Scene::Scene(const Renderer& renderer) :
	m_Renderer(&renderer), m_DescriptorsPool(renderer.makeDescriptorPool()), 
	m_ObjectDSL(m_Renderer->makeDescriptorSetLayout()), m_Camera(renderer, m_DescriptorsPool)
{
	setModified();

	m_ObjectDSL.addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_ObjectDSL.addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_ObjectDSL.create();

	std::vector<DescriptorSetLayout*> layouts{};
	layouts.push_back(m_Camera.getDescriptorSetLayout());
	layouts.push_back(&m_ObjectDSL);

	m_ObjectLists.emplace_back(renderer, std::string("res/shaders/baseVert.spv"), std::string("res/shaders/baseFrag.spv"), layouts);

}

void Scene::render(RenderTarget target)
{
	if (target.updated())
	{
		setModified();
	}

	auto [index, count] = target.getFrameInfo();
	if (m_FrameModified.size() < count)
	{
		m_FrameModified.resize(count);
		m_DescriptorsPool.setFrameCount(count);
		setModified();
	}

	if (m_FrameModified[index])
	{
		recordCommandBuffer(target);
		m_FrameModified[index] = false;
	}

	updateUniforms(target);
}

Scene::~Scene()
{
}

void Scene::recordCommandBuffer(RenderTarget& target)
{
	target.beginCommandRecording();

	for (auto& objectList : m_ObjectLists)
	{
		target.bindPipeline(objectList.getPipeline());

		target.bindDescriptorSet(objectList.getPipeline(), m_Camera.getDescriptorSet());

	}

	// VkBuffer vertexBuffers[] = { m_VertexBuffer };
	// VkDeviceSize offsets[] = { 0 };
	// vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	// vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[currentFrame], 0, nullptr);
	// vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 2, 0, 0, 0);

	target.endCommandRecording();
}

void Scene::updateUniforms(RenderTarget& target)
{

}

void Scene::setModified()
{
	for (size_t i = 0; i < m_FrameModified.size(); i++)
	{
		m_FrameModified[i] = true;
	}
}

} // namespace vulture
