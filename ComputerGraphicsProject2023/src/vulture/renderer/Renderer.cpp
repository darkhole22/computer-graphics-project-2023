#include "Renderer.h"
#include "RenderTarget.h"

namespace vulture {

Renderer::Renderer(const Window& window) :
	m_Instance(window.getName()), m_Surface(m_Instance, window), m_Device(m_Instance, m_Surface),
	m_RenderPass(m_Device, m_Surface), m_SwapChain(m_Device, m_Surface, m_RenderPass)
{
	m_SwapChain.addCallback([this](const SwapChainRecreatedEvent& e) {
		m_SwapChainRecreated = true;
	});
}

RenderTarget Renderer::getRenderTarget()
{
	uint32_t frame = m_CurrentFrame;
	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	
	return RenderTarget(m_SwapChain, m_Device, frame, m_SwapChainRecreated);
	m_SwapChainRecreated = false;
}

Renderer::~Renderer()
{
	m_Device.waitIdle();
}

} // namespace vulture
