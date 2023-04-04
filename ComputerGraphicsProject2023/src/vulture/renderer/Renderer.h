#pragma once

#include "Window.h"
#include "vulkan_wrapper.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace vulture {

class RenderTarget;

struct Vertex
{
	glm::vec3 pos = { 0, 0, 0 };
	glm::vec3 norm = { 0, 0 , 0 };
	glm::vec2 texCoord = { 0, 0 };
};

class Renderer
{
public:
	Renderer(const Window& window);

	RenderTarget getRenderTarget();

	// bind pipeline / shaders
	// render skybox
	// render object

	// update descriptor sets

	inline const RenderPass& getRenderPass() const { return m_RenderPass; }
	inline DescriptorPool makeDescriptorPool() const { return DescriptorPool(m_Device, m_SwapChain.getImageCount()); }
	inline DescriptorSetLayout makeDescriptorSetLayout() const { return DescriptorSetLayout(m_Device); }
	template <class T> inline Uniform<T> makeUniform() const { return Uniform<T>(m_Device, m_SwapChain.getImageCount()); }

	inline static const VertexLayout getVertexLayout()
	{
		return VertexLayout(sizeof(Vertex), {
			{VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, pos))},
			{VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, norm))},
			{VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, texCoord))}
		});
	}

	~Renderer();
private:
	Instance m_Instance;
	Surface m_Surface;
	Device m_Device;
	RenderPass m_RenderPass;
	SwapChain m_SwapChain;

	uint32_t m_CurrentFrame = 0;
	bool m_SwapChainRecreated = false;
};

} // namespace vulture