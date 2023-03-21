#pragma once

#include <renderer/Window.h>
#include <renderer/vulkan_wrapper.h>

namespace computergraphicsproject {

class RenderTarget;

class Renderer
{
public:
	Renderer(const Window& window);

	RenderTarget getRenderTarget();

	// bind pipeline / shaders
	// render skybox
	// render object

	// update descriptor sets

	~Renderer();
private:
	Instance m_Instance;
	Surface m_Surface;
	// PhysicalDevice m_PhysicalDevice;
	Device m_Device;
	RenderPass m_RenderPass;
	SwapChain m_SwapChain;

	uint32_t m_CurrentFrame = 0;
};

}