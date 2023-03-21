#include "RenderTarget.h"

#include <iostream>

namespace computergraphicsproject {

RenderTarget::RenderTarget(SwapChain& swapChain, const Device& divice, uint32_t currentFrame) :
	m_SwapChain(&swapChain), m_CurrentFrame(currentFrame), m_CommandBuffer(&swapChain.getCommandBuffer(currentFrame))
{
	m_ImageIndex = m_SwapChain->getImageIndex(currentFrame);
}

void RenderTarget::beginCommandRecording()
{
	m_CommandBuffer->reset();
	m_CommandBuffer->begin();
	m_CommandBuffer->beginRenderPass(*m_SwapChain->m_RenderPass, 
		m_SwapChain->m_Framebuffers[m_CurrentFrame], m_SwapChain->m_Extent);
}

void RenderTarget::endCommandRecording()
{
	m_CommandBuffer->endRenderPass();
	m_CommandBuffer->end();
}

RenderTarget::~RenderTarget()
{
	m_SwapChain->submit(m_CurrentFrame, m_ImageIndex);
}

}