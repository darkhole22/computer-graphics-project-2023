#pragma once

#include <renderer/Window.h>
#include <renderer/vulkan_wrapper.h>

namespace computergraphicsproject {

class Renderer
{
public:
	Renderer(const Window& window);

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

};

}