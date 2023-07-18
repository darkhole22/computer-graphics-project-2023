#pragma once

#include "Window.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "DescriptorSet.h"
#include "Pipeline.h"
#include "Model.h"

namespace vulture {

class FrameContext;

struct ResourceInfo
{
	String path;
};

struct RendererData
{
	ResourceInfo resourceInfo;

	SwapChain* swapChain = nullptr;
	RenderPass* renderPass = nullptr;

	u32 currentFrame = 0;
};

class Renderer
{
public:
	static bool init(const String& applicationName, const Window& window);

	static void cleanup();

	static FrameContext getFrameContext();

	static void waitIdle();

	static const RenderPass& getRenderPass();
	static inline DescriptorPool makeDescriptorPool() { return DescriptorPool(getImageCount()); }
	
	template <class T> static inline Uniform<T> makeUniform() { return Uniform<T>(getImageCount()); }

	static inline const VertexLayout getVertexLayout()
	{
		return VertexLayout(sizeof(Vertex), {
			{VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, pos))},
			{VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, norm))},
			{VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, texCoord))}
		});
	}
private:
	static u32 getImageCount();
};

} // namespace vulture