#pragma once

#include <vector>
#include <filesystem>

#include "vulture/renderer/FrameContext.h"
#include "vulture/scene/Camera.h"
#include "vulture/scene/Skybox.h"
#include "vulture/scene/ui/UIHandler.h"
#include "vulture/scene/physics/CollisionEngine.h"
#include "vulture/scene/tool/Tween.h"
#include "vulture/scene/tool/Timer.h"
#include "vulture/scene/GameObject.h"
#include "vulture/scene/World.h"

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

	PipelineHandle makePipeline(const String& vertexShader, const String& fragmentShader, Ref<DescriptorSetLayout> descriptorSetLayout);

	ObjectHandle addObject(PipelineHandle pipeline, Ref<Model> model, Ref<DescriptorSet> descriptorSet);
	void removeObject(PipelineHandle pipeline, ObjectHandle obj);

	/**
	* @brief Set the skybox texture to the one specified.
	* If name equals the empty string than the skybox is disabled.
	*
	* @param name: Name of the cubemap texture to use.
	*/
	void setSkybox(const String& name);

	Camera* getCamera() { return &m_Camera; }
	UIHandler* getUIHandle() { return &m_UIHandler; }
	World* getWorld() { return &m_World; }
	DescriptorPool* getDescriptorPool() { return &m_DescriptorsPool; }

	Ref<Tween> makeTween();
	Ref<Timer> makeTimer(f32 waitTime, bool oneShot = true);

	void callLater(std::function<void()> fun);

	inline void addHitbox(Ref<HitBox> hitbox) { m_CollisionEngine.addHitbox(hitbox); }
	inline void removeHitbox(Ref<HitBox> hitbox) { m_CollisionEngine.removeHitbox(hitbox); }

	~Scene() = default;
private:
	DescriptorPool m_DescriptorsPool;

	Camera m_Camera;
	Skybox m_Skybox;
	World m_World;
	UIHandler m_UIHandler;
	CollisionEngine m_CollisionEngine;

	std::vector<bool> m_FrameModified;
	std::unordered_map<PipelineHandle, SceneObjectList> m_ObjectLists;

	std::unordered_map<ObjectHandle, Ref<GameObject>> m_GameObjects;

	PipelineHandle m_NextPipelineHandle = 0;

	std::unordered_set<Ref<Tween>> m_Tweens;
	bool m_TweenLoopFlag = false;
	std::unordered_set<Ref<Timer>> m_Timers;
	bool m_TimersLoopFlag = false;

	std::vector<std::function<void()>> m_DeferredFunctions;

	Ref<DescriptorSetLayout> m_GameObjectDSL;
	PipelineHandle m_GameObjectPipeline;

	void recordCommandBuffer(FrameContext& target);
	void updateUniforms(FrameContext& target);

	void setModified();
};

}
