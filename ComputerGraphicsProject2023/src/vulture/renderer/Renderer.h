#pragma once

#include "Window.h"
#include "vulkan_wrapper.h"
#include "Model.h"


namespace vulture {

class RenderTarget;
/*
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
struct Vertex
{
	glm::vec3 pos = { 0, 0, 0 };
	glm::vec3 norm = { 0, 0 , 0 };
	glm::vec2 texCoord = { 0, 0 };
};

struct VertexBuilder
{
	Vertex operator()(const Model::_BaseVertex& vertex)
	{
		Vertex v;

		return v;
	}
};
*/

class Renderer
{
public:
	Renderer(const Window& window);

	RenderTarget getRenderTarget();

	// bind pipeline / shaders
	// render skybox
	// render object

	// update descriptor sets

	inline void waitIdle() const { m_Device.waitIdle(); }

	inline const Device& getDevice() const { return m_Device; }
	inline const RenderPass& getRenderPass() const { return m_RenderPass; }
	inline DescriptorPool makeDescriptorPool() const { return DescriptorPool(m_Device, m_SwapChain.getImageCount()); }
	inline Ref<DescriptorSetLayout> makeDescriptorSetLayout() const { return Ref<DescriptorSetLayout>(new DescriptorSetLayout(m_Device)); }
	template <class T> inline Uniform<T> makeUniform() const { return Uniform<T>(m_Device, m_SwapChain.getImageCount()); }
	inline Ref<Texture> makeTexture(const String& path) const { return Ref<Texture>(new Texture(m_Device, path)); }
	inline Ref<Model> makeBaseModel(const String& modelPath) const 
	{
		// return Ref<Model>(Model::make<Vertex, VertexBuilder>(m_Device, modelPath));
		return Ref<Model>(Model::make(m_Device, modelPath));
	}

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