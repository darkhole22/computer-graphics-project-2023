#pragma once

#include <bitset>
#include <vector>

#include "renderer/RenderTarget.h"
// #include "scene/GameObject.h" TODO

namespace computergraphicsproject {

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
	Scene(const Renderer& renderer);

	void render(RenderTarget target);
private:
	Renderer const* m_Renderer;

	std::vector<bool> m_FrameModified;
	std::vector<SceneObjectList> m_ObjectLists;

	void recordCommandBuffer(RenderTarget& target);

	void setModified();
};

} // namespace computergraphicsproject
