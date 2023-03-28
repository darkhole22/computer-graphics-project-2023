#pragma once

#include <renderer/Window.h>
#include <renderer/vulkan_wrapper.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace computergraphicsproject {

class RenderTarget;

struct Vertex
{
	glm::vec3 pos = { 0, 0, 0 };
	glm::vec3 color = { 0, 0 , 0 };
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

	inline static const VertexLayout getVertexLayout()
	{
		return VertexLayout(sizeof(Vertex), {
			{VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)},
			{VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
			{VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord)}
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

}