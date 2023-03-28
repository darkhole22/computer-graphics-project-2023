#include "RenderTarget.h"

#include <iostream>

namespace computergraphicsproject {

RenderTarget::RenderTarget(SwapChain& swapChain, const Device& device, uint32_t currentFrame, bool swapChainRecreated) :
	m_SwapChain(&swapChain), m_CurrentFrame(currentFrame), m_ImageIndex(m_SwapChain->getImageIndex(currentFrame)),
	m_CommandBuffer(&swapChain.getCommandBuffer(m_ImageIndex)), m_ImageCount(m_SwapChain->getImageCount()),
	m_SwapChainRecreated(swapChainRecreated)
{
}

void RenderTarget::beginCommandRecording()
{
	m_CommandBuffer->reset();
	m_CommandBuffer->begin();
	m_CommandBuffer->beginRenderPass(*m_SwapChain->m_RenderPass, 
		m_SwapChain->m_Framebuffers[m_ImageIndex], m_SwapChain->m_Extent);
}

void RenderTarget::endCommandRecording()
{
	m_CommandBuffer->endRenderPass();
	m_CommandBuffer->end();
}

void RenderTarget::bindPipeline(const Pipeline& pipeline)
{
	m_CommandBuffer->bindPipeline(pipeline, *m_SwapChain);
}

RenderTarget::~RenderTarget()
{
	m_SwapChain->submit(m_CurrentFrame, m_ImageIndex);
}

}