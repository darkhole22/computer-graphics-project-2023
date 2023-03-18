#include "Renderer.h"

namespace computergraphicsproject {

Renderer::Renderer(const Window& window) :
	m_Instance(window.getName()), m_Surface(m_Instance, window), m_Device(m_Instance, m_Surface),
	m_RenderPass(m_Device, m_Surface), m_SwapChain(m_Device, m_Surface, m_RenderPass)
{
}

Renderer::~Renderer()
{
	m_Device.waitIdle();
}

}
