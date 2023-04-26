#pragma once

#include <vector>
#include <filesystem>

#include "vulture/renderer/RenderTarget.h"
#include "vulture/scene/Camera.h"

#include "GameObject.h"
#include "vulture/scene/ui/UIHandler.h"

namespace vulture {

class RenderableObject
{
public:
	RenderableObject(Ref<Model> model, Ref<DescriptorSet> descriptorSet);

	inline const DescriptorSet& getDescriptorSet() { return *m_DescriptorSet; }
	inline const Model& getModel() { return *m_Model.get(); }
private:
	Ref<Model> m_Model;
	Ref<DescriptorSet> m_DescriptorSet;
};

using ObjectHandle = int64_t;

class SceneObjectList
{
public:
	SceneObjectList(const Renderer& renderer, const std::string& vertexShader, const std::string& fragmentShader,
		const std::vector<DescriptorSetLayout*>& descriptorSetLayouts);
	
	inline const Pipeline& getPipeline() const { return *m_Pipeline; }
	ObjectHandle addObject(RenderableObject obj);
	void removeObject(ObjectHandle handle);

	auto begin() { return m_Objects.begin(); }
	auto end() { return m_Objects.end(); }
private:
	Ref<Pipeline> m_Pipeline;
	ObjectHandle m_NextObjectHandle = 0;
	std::unordered_map<ObjectHandle, RenderableObject> m_Objects;
};

using PipelineHandle = int64_t;

class Scene
{
public:
	explicit Scene(const Renderer& renderer);

	void render(RenderTarget target, float dt);

	PipelineHandle makePipeline(const std::string& vertexShader, const std::string& fragmentShader, Ref<DescriptorSetLayout> descriptorSetLayout);
	ObjectHandle addObject(PipelineHandle pipeline, Ref<Model> model, Ref<DescriptorSetLayout> layout, const std::vector<DescriptorWrite>& descriptorWrites);

	Ref<GameObject> makeObject(const std::string& modelPath, const std::string& texturePath);
	Camera* getCamera() { return &m_Camera; }
	UIHandler* getUIHandle() { return &m_UIHandler; }

	~Scene() = default;
private:
	Renderer const* m_Renderer;
	DescriptorPool m_DescriptorsPool;
	
	Camera m_Camera;
	UIHandler m_UIHandler;

	std::vector<bool> m_FrameModified;

	PipelineHandle m_NextPipelineHandle = 0;
	std::unordered_map<PipelineHandle, SceneObjectList> m_ObjectLists;

	Ref<DescriptorSetLayout> m_GameObjectDSL;
	PipelineHandle m_GameObjectPipeline;

	std::unordered_map<ObjectHandle, Ref<GameObject>> m_GameObjects;

	void recordCommandBuffer(RenderTarget& target);
	void updateUniforms(RenderTarget& target);

	void setModified();

	void removeObject(const Ref <vulture::GameObject> &obj);
};

}
