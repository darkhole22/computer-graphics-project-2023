#pragma once

#include <vector>
#include <filesystem>

#include "vulture/renderer/FrameContext.h"
#include "vulture/scene/Camera.h"
#include "vulture/scene/ui/UIHandler.h"
#include "vulture/scene/Tween.h"

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
	SceneObjectList(const String& vertexShader, const String& fragmentShader,
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

using PipelineHandle = i64;

class Scene
{
public:
	Scene();

	void render(FrameContext target, float dt);

	PipelineHandle makePipeline(const String& vertexShader, const String& fragmentShader, Ref<DescriptorSetLayout> descriptorSetLayout);
	ObjectHandle addObject(PipelineHandle pipeline, Ref<Model> model, Ref<DescriptorSetLayout> layout, const std::vector<DescriptorWrite>& descriptorWrites);

	Camera* getCamera() { return &m_Camera; }
	UIHandler* getUIHandle() { return &m_UIHandler; }

	Ref<Tween> makeTween();

	~Scene() = default;
private:
	DescriptorPool m_DescriptorsPool;
	
	Camera m_Camera;
	UIHandler m_UIHandler;

	std::vector<bool> m_FrameModified;
	PipelineHandle m_NextPipelineHandle = 0;
	std::unordered_map<PipelineHandle, SceneObjectList> m_ObjectLists;

	std::unordered_set<Ref<Tween>> m_Tweens;

	void recordCommandBuffer(FrameContext& target);
	void updateUniforms(FrameContext& target);

	void setModified();
};

}
