#include "Renderer.h"
#include "FrameContext.h"

#include "VulkanContext.h"

namespace vulture {

extern VulkanContextData vulkanData;

struct RendererData
{
	SwapChain* swapChain = nullptr;
	RenderPass* renderPass = nullptr;

	uint32_t currentFrame = 0;
};

static RendererData rendererData = {};

bool Renderer::init(const String& applicationName, const Window& window)
{
	if (!VulkanContext::init(applicationName, window))
		return false;

	rendererData.swapChain = new SwapChain(window);
	rendererData.renderPass = new RenderPass(rendererData.swapChain->getImageFormat());
	if (!rendererData.swapChain->attachRenderPass(*rendererData.renderPass))
		return false;

	return true;
}

void Renderer::cleanup()
{
	vkDeviceWaitIdle(vulkanData.device);

	delete rendererData.swapChain;
	delete rendererData.renderPass;

	VulkanContext::cleanup();
}

FrameContext Renderer::getFrameContext()
{
	uint32_t frame = rendererData.currentFrame;
	rendererData.currentFrame = (rendererData.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	
	return FrameContext(*rendererData.swapChain, frame, rendererData.swapChain->wasRecreated());
}

void Renderer::waitIdle()
{
	vkDeviceWaitIdle(vulkanData.device);
}

const RenderPass& Renderer::getRenderPass()
{
	return *rendererData.renderPass;
}

u32 Renderer::getImageCount()
{
	return rendererData.swapChain->getImageCount();
}

} // namespace vulture
