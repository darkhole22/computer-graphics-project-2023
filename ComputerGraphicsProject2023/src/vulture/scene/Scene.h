#pragma once

#include <vector>

#include "vulture/renderer/RenderTarget.h"
// #include "scene/GameObject.h" TODO

namespace vulture {

class SceneObjectList
{
public:
	SceneObjectList(const Renderer& renderer, const std::string& vertexShader, const std::string& fragmentShader,
		const DescriptorSetLayout& descriptorSetLayout);
	
	inline const Pipeline& getPipeline() const { return m_Pipeline; }

private:
	Pipeline m_Pipeline;
	// std::vector<GameObject> m_Objects; TODO
};

class Scene
{
public:
	explicit Scene(const Renderer& renderer);

	void render(RenderTarget target);
private:
	Renderer const* m_Renderer;

	std::vector<bool> m_FrameModified;
	std::vector<SceneObjectList> m_ObjectLists;

	void recordCommandBuffer(RenderTarget& target);

	void setModified();
};

}
