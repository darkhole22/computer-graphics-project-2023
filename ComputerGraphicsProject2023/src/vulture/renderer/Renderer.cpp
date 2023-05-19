#include "Renderer.h"
#include "FrameContext.h"

#include "VulkanContext.h"

namespace vulture {

extern VulkanContextData vulkanData;

RendererData rendererData = {};

bool Renderer::init(const String& applicationName, const Window& window)
{
	rendererData.resourceInfo.path = "res/";

	if (!VulkanContext::init(applicationName, window))
		return false;

	rendererData.swapChain = new SwapChain(window);
	rendererData.renderPass = new RenderPass(rendererData.swapChain->getImageFormat());
	if (!rendererData.swapChain->attachRenderPass(*rendererData.renderPass))
		return false;

	if (!Texture::init())
		return false;

	if (!Model::init())
		return false;

	return true;
}

void Renderer::cleanup()
{
	vkDeviceWaitIdle(vulkanData.device);

	Model::cleanup();
	Texture::cleanup();

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
