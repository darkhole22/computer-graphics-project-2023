#include "FrameContext.h"

#include <iostream>

namespace vulture {

FrameContext::FrameContext(SwapChain& swapChain, uint32_t currentFrame, bool swapChainRecreated) :
	m_SwapChain(&swapChain), m_CurrentFrame(currentFrame), m_ImageIndex(m_SwapChain->getImageIndex(currentFrame)),
	m_CommandBuffer(&swapChain.getCommandBuffer(m_ImageIndex)), m_ImageCount(m_SwapChain->getImageCount()),
	m_SwapChainRecreated(swapChainRecreated)
{
}

void FrameContext::beginCommandRecording()
{
	m_CommandBuffer->reset();
	m_CommandBuffer->begin();
	m_CommandBuffer->beginRenderPass(*m_SwapChain->m_RenderPass, 
		m_SwapChain->m_Framebuffers[m_ImageIndex], m_SwapChain->m_Extent);
}

void FrameContext::endCommandRecording()
{
	m_CommandBuffer->endRenderPass();
	m_CommandBuffer->end();
}

void FrameContext::bindPipeline(const Pipeline& pipeline)
{
	m_CommandBuffer->bindPipeline(pipeline, *m_SwapChain);
}

void FrameContext::bindDescriptorSet(const Pipeline& pipeline, const DescriptorSet& descriptorSet, u32 set)
{
	m_CommandBuffer->bindDescriptorSet(pipeline, descriptorSet.getHandle(m_ImageIndex), set);
}

void FrameContext::drawModel(const Model& model)
{
	m_CommandBuffer->bindVertexBuffer(model.getVertexBuffer());

	m_CommandBuffer->bindIndexBuffer(model.getIndexBuffer());

	m_CommandBuffer->drawIndexed(model.getIndexCount());
}

void FrameContext::bindVertexBuffer(const Buffer& buffer)
{
	m_CommandBuffer->bindVertexBuffer(buffer);
}

void FrameContext::bindIndexBuffer(const Buffer& buffer)
{
	m_CommandBuffer->bindIndexBuffer(buffer);
}

void FrameContext::drawIndexed(u32 count)
{
	m_CommandBuffer->drawIndexed(count);
}

FrameContext::~FrameContext()
{
	m_SwapChain->submit(m_CurrentFrame, m_ImageIndex);
}

} // namespace vulture