#pragma once

#include <vector>
#include <filesystem>

#include "vulture/renderer/RenderTarget.h"
#include "vulture/scene/Camera.h"

namespace vulture {

class RenderableObject
{
public:
	RenderableObject(const Pipeline& pipeline, const std::filesystem::path& path);

private:

};

class SceneObjectList
{
public:
	SceneObjectList(const Renderer& renderer, const std::string& vertexShader, const std::string& fragmentShader,
		const std::vector<DescriptorSetLayout*>& descriptorSetLayouts);
	
	inline const Pipeline& getPipeline() const { return *m_Pipeline; }
private:
	std::shared_ptr<Pipeline> m_Pipeline;
	std::vector<RenderableObject> m_Objects;
};

class Scene
{
public:
	explicit Scene(const Renderer& renderer);

	void render(RenderTarget target);

	~Scene();
private:
	Renderer const* m_Renderer;
	DescriptorPool m_DescriptorsPool;
	DescriptorSetLayout m_ObjectDSL;
	
	Camera m_Camera;

	std::vector<bool> m_FrameModified;
	std::vector<SceneObjectList> m_ObjectLists; // TODO consider changing this to an unordered_set

	void recordCommandBuffer(RenderTarget& target);
	void updateUniforms(RenderTarget& target);

	void setModified();
};

}
