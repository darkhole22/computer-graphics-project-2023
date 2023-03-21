#include "Scene.h"

#include <iostream>

namespace computergraphicsproject {

Scene::Scene() :
	m_FrameModified()
{
}

void Scene::render(RenderTarget target)
{
	uint32_t currentFrame = target.getCurrentFrame();
	if (m_FrameModified.test(currentFrame))
	{
		recordCommandBuffer(target);
		m_FrameModified.reset(currentFrame);
	}

	// updateUniformBuffer(target);
}

void Scene::recordCommandBuffer(RenderTarget& target)
{
	target.beginCommandRecording();

	// vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

	// VkViewport viewport{};
	// viewport.x = 0.0f;
	// viewport.y = 0.0f;
	// viewport.width = static_cast<float>(m_SwapChainExtent.width);
	// viewport.height = static_cast<float>(m_SwapChainExtent.height);
	// viewport.minDepth = 0.0f;
	// viewport.maxDepth = 1.0f;
	// vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	// VkRect2D scissor{};
	// scissor.offset = { 0, 0 };
	// scissor.extent = m_SwapChainExtent;
	// vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// VkBuffer vertexBuffers[] = { m_VertexBuffer };
	// VkDeviceSize offsets[] = { 0 };
	// vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[currentFrame], 0, nullptr);

	// vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 2, 0, 0, 0);

	target.endCommandRecording();
}



} // namespace computergraphicsproject
