#pragma once

#include "Window.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "DescriptorSet.h"
#include "Pipeline.h"
#include "Model.h"

namespace vulture {

class FrameContext;

class Renderer
{
public:
	static bool init(const String& applicationName, const Window& window);

	static void cleanup();

	static FrameContext getFrameContext();

	// bind pipeline / shaders
	// render skybox
	// render object

	// update descriptor sets

	static void waitIdle(); // { m_Device.waitIdle(); }

	static const RenderPass& getRenderPass();// { return m_RenderPass; }
	static inline DescriptorPool makeDescriptorPool() { return DescriptorPool(getImageCount()); }
	// inline Ref<DescriptorSetLayout> makeDescriptorSetLayout() const { return Ref<DescriptorSetLayout>(new DescriptorSetLayout(m_Device)); }
	template <class T> static inline Uniform<T> makeUniform() { return Uniform<T>(getImageCount()); }
	// inline Ref<Texture> makeTexture(const String& path) const { return Ref<Texture>(new Texture(m_Device, path)); }

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