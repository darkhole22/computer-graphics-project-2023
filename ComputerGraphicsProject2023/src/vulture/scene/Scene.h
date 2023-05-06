#pragma once

#include <vector>
#include <filesystem>

#include "vulture/renderer/FrameContext.h"
#include "vulture/scene/Camera.h"
#include "vulture/scene/Skybox.h"
#include "vulture/scene/ui/UIHandler.h"
#include "vulture/scene/Tween.h"
#include "vulture/scene/GameObject.h"

namespace vulture {

using PipelineHandle = i64;

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


class SceneObjectList
{
public:
	SceneObjectList(const String& vertexShader, const String& fragmentShader,
		const std::vector<DescriptorSetLayout*>& descriptorSetLayouts);
	
	inline const Pipeline& getPipeline() const { return *m_Pipeline; }
	void addObject(ObjectHandle handle, const RenderableObject& obj);
	void removeObject(ObjectHandle handle);

	auto begin() { return m_Objects.begin(); }
	auto end() { return m_Objects.end(); }
private:
	Ref<Pipeline> m_Pipeline;
	std::unordered_map<ObjectHandle, RenderableObject> m_Objects;
};


class Scene
{
public:
	Scene();

	void render(FrameContext target, float dt);

	void addObject(Ref<GameObject> obj);
	void removeObject(Ref<GameObject> obj);

	void setSkybox(const String& name);

	Camera* getCamera() { return &m_Camera; }
	UIHandler* getUIHandle() { return &m_UIHandler; }

	Ref<Tween> makeTween();

	~Scene() = default;
private:
	DescriptorPool m_DescriptorsPool;
	
	Camera m_Camera;
	Skybox m_Skybox;
	UIHandler m_UIHandler;

	std::vector<bool> m_FrameModified;
	std::unordered_map<PipelineHandle, SceneObjectList> m_ObjectLists;

	std::unordered_map<ObjectHandle, Ref<GameObject>> m_GameObjects;

	PipelineHandle m_NextPipelineHandle = 0;

	std::unordered_set<Ref<Tween>> m_Tweens;


	Ref<DescriptorSetLayout> m_GameObjectDSL;
	PipelineHandle m_GameObjectPipeline;

	PipelineHandle makePipeline(const String& vertexShader, const String& fragmentShader, Ref<DescriptorSetLayout> descriptorSetLayout);
	
	void recordCommandBuffer(FrameContext& target);
	void updateUniforms(FrameContext& target);

	void setModified();
};

}
