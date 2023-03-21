#include "Renderer.h"

#include "RenderTarget.h"

namespace computergraphicsproject {

Renderer::Renderer(const Window& window) :
	m_Instance(window.getName()), m_Surface(m_Instance, window), m_Device(m_Instance, m_Surface),
	m_RenderPass(m_Device, m_Surface), m_SwapChain(m_Device, m_Surface, m_RenderPass)
{
}

RenderTarget Renderer::getRenderTarget()
{
	uint32_t frame = m_CurrentFrame;
	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return RenderTarget(m_SwapChain, m_Device, frame);
}

Renderer::~Renderer()
{
	m_Device.waitIdle();
}

}
