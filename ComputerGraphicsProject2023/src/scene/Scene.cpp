#include "Scene.h"

#include <iostream>

namespace computergraphicsproject {

SceneObjectList::SceneObjectList(const Renderer& renderer, const std::string& vertexShader, 
	const std::string& fragmentShader, const DescriptorSetLayout& descriptorSetLayout) :
	m_Pipeline(renderer.getRenderPass(), vertexShader, fragmentShader, descriptorSetLayout, Renderer::getVertexLayout())
{
	
}

Scene::Scene(const Renderer& renderer) :
	m_Renderer(&renderer), m_FrameModified()
{
	setModified();
}

void Scene::render(RenderTarget target)
{
	if (target.justUpdated())
	{
		setModified();
	}

	auto [index, count] = target.getFrameInfo();
	if (m_FrameModified.size() < count)
	{
		m_FrameModified.resize(count);
		setModified();
	}
	if (m_FrameModified[index])
	{
		recordCommandBuffer(target);
		m_FrameModified[index] = false;
	}

	// updateUniformBuffer(target);
}

void Scene::recordCommandBuffer(RenderTarget& target)
{
	target.beginCommandRecording();

	for (auto& objectList : m_ObjectLists)
	{
		target.bindPipeline(objectList.getPipeline());



	}



	// VkBuffer vertexBuffers[] = { m_VertexBuffer };
	// VkDeviceSize offsets[] = { 0 };
	// vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	// vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[currentFrame], 0, nullptr);
	// vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 2, 0, 0, 0);

	target.endCommandRecording();
}

void Scene::setModified()
{
	for (size_t i = 0; i < m_FrameModified.size(); i++)
	{
		m_FrameModified[i] = true;
	}
}

} // namespace computergraphicsproject
