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

/**
 * @class RenderableObject
 * @brief Represents a renderable object in the scene with an associated model and descriptor set.
 */
class RenderableObject
{
public:
	/**
	 * @brief Constructor for the `RenderableObject` class.
	 *
	 * @param model The model associated with the renderable object.
	 * @param descriptorSet The descriptor set associated with the renderable object.
	 */
	RenderableObject(Ref<Model> model, Ref<DescriptorSet> descriptorSet);

	/**
	 * @brief Gets the descriptor set associated with the renderable object.
	 *
	 * @return The reference to the descriptor set associated with the renderable object.
	 */
	inline const DescriptorSet& getDescriptorSet() { return *m_DescriptorSet; }

	/**
	 * @brief Gets the model associated with the renderable object.
	 *
	 * @return The reference to the model associated with the renderable object.
	 */
	inline const Model& getModel() { return *m_Model.get(); }
private:
	Ref<Model> m_Model;
	Ref<DescriptorSet> m_DescriptorSet;
};

/**
 * @class SceneObjectList
 * @brief Represents a list of renderable objects with a shared rendering pipeline.
 */
class SceneObjectList
{
public:
	/**
	 * @brief Constructor for the SceneObjectList class.
	 *
	 * @param vertexShader The name of the vertex shader to use for rendering.
	 * @param fragmentShader The  name of the fragment shader to use for rendering.
	 * @param descriptorSetLayouts The list of descriptor set layouts associated with the rendering pipeline.
	 * @param config Advanced configurations of the pipeline
	 */
	SceneObjectList(const String& vertexShader, const String& fragmentShader,
					const std::vector<DescriptorSetLayout*>& descriptorSetLayouts, PipelineAdvancedConfig config);

	/**
	 * @brief Gets the pipeline associated with the scene object list.
	 *
	 * @return The reference to the pipeline associated with the scene object list.
	 */
	inline const Pipeline& getPipeline() const { return *m_Pipeline; }

	/**
	 * @brief Adds a renderable object to the scene object list.
	 *
	 * @param handle The handle of the object to be added.
	 * @param obj The renderable object to be added to the list.
	 */
	void addObject(ObjectHandle handle, const RenderableObject& obj);

	/**
	 * @brief Removes a renderable object from the scene object list.
	 *
	 * @param handle The handle of the object to be removed.
	 */
	void removeObject(ObjectHandle handle);

	/**
	 * @brief Gets the iterator pointing to the beginning of the scene object list.
	 *
	 * @return The iterator pointing to the beginning of the scene object list.
	 */
	auto begin() { return m_Objects.begin(); }

	/**
	 * @brief Gets the iterator pointing to the end of the scene object list.
	 *
	 * @return The iterator pointing to the end of the scene object list.
	 */
	auto end() { return m_Objects.end(); }
private:
	Ref<Pipeline> m_Pipeline;
	std::unordered_map<ObjectHandle, RenderableObject> m_Objects;
};

/**
 * @class Scene
 * @brief Represents a scene containing renderable objects, associated resources and utilities to handle game logic.
 */
class Scene
{
public:
	Scene();

	/**
	 * @brief Renders the scene using the provided frame context and delta time.
	 *
	 * @param target The frame context for rendering the scene.
	 * @param dt The delta time since the last frame.
	 */
	void render(FrameContext target, f32 dt);

	/**
	 * @brief Adds a game object to the scene using the default Phong pipeline.
	 *
	 * @param obj The reference to the game object to be added.
	 */
	void addObject(Ref<GameObject> obj);

	/**
	 * @brief Adds a game object to the scene using the default Phong pipeline.
	 *
	 * @param obj The reference to the game object to be added.
	 * @param pipelineHandle Pipeline of the object to be added
	 */
	void addObject(Ref<GameObject> obj, PipelineHandle pipelineHandle);

	/**
	 * @brief Removes a game object to the scene.
	 *
	 * @param obj The reference to the game object to be removed.
	 */
	void removeObject(Ref<GameObject> obj);

	/**
	 * @brief Removes a game object to the scene.
	 *
	 * @param obj The reference to the game object to be removed.
	 * @param pipelineHandle Pipeline of the object to be removed
	 */
	void removeObject(Ref<GameObject> obj, PipelineHandle pipelineHandle);

	/**
	 * @brief Adds a new object to the scene using a custom pipeline.
	 *
	 * @param pipeline The handle of the rendering pipeline to use for the object.
	 * @param model The model associated with the object.
	 * @param descriptorSet The descriptor set associated with the object.
	 * @return The handle of the newly added object.
	 */
	ObjectHandle addObject(PipelineHandle pipeline, Ref<Model> model, Ref<DescriptorSet> descriptorSet);

	/**
	 * @brief Removes an object from the scene.
	 *
	 * @param pipeline The handle of the rendering pipeline to which the object belongs.
	 * @param obj The handle of the object to be removed.
	 */
	void removeObject(PipelineHandle pipeline, ObjectHandle obj);

	/**
	 * @brief Creates a new rendering pipeline with the specified vertex and fragment shaders.
	 *
	 * @param vertexShader The name of the vertex shader to use in the pipeline.
	 * @param fragmentShader The name of the fragment shader to use in the pipeline.
	 * @param descriptorSetLayout The descriptor set layout associated with the pipeline.
	 * @param config Advanced configurations of the pipeline
	 * @return The handle of the newly created rendering pipeline.
	 */
	PipelineHandle makePipeline(const String& vertexShader, const String& fragmentShader, Ref<DescriptorSetLayout> descriptorSetLayout, PipelineAdvancedConfig config = PipelineAdvancedConfig::defaultConfig);

	/**
	* @brief Set the skybox texture for the scene.
	* If the name is an empty string, the skybox is disabled.
	*
	* @param name Name of the cubemap texture to use for the skybox.
	*/
	void setSkybox(const String& name);

	/**
	 * @brief Creates a new tween for animation and returns a reference to it.
	 *
	 * @return The reference to the newly created tween.
	 */
	Ref<Tween> makeTween();

	/**
	 * @brief Creates a new timer for delayed actions and returns a reference to it.
	 *
	 * @param waitTime The time (in seconds) to wait before triggering the timer.
	 * @param oneShot If true, the timer will only trigger once; otherwise, it will repeat.
	 * @return The reference to the newly created timer.
	 */
	Ref<Timer> makeTimer(f32 waitTime, bool oneShot = true);

	/**
	 * @brief Schedules a function to be called at the end of the current frame.
	 *
	 * If this is called inside a deferred function, the provided function will be
	 * scheduled to run at the end of the next frame.
	 *
	 * @param fun The function to be called.
	 */
	void callLater(std::function<void()> fun);

	/**
	 * @brief Adds a hitbox to the collision engine for physics calculations.
	 *
	 * @param hitbox The reference to the hitbox to be added.
	 */
	inline void addHitbox(Ref<HitBox> hitbox) { m_CollisionEngine.addHitbox(hitbox); }

	/**
	 * @brief Removes a hitbox from the collision engine.
	 *
	 * @param hitbox The reference to the hitbox to be removed.
	 */
	inline void removeHitbox(Ref<HitBox> hitbox) { m_CollisionEngine.removeHitbox(hitbox); }

	/**
	 * @brief Gets a pointer to the camera used in the scene.
	 *
	 * @return A pointer to the camera used in the scene.
	 */
	Camera* getCamera() { return &m_Camera; }

	/**
	 * @brief Gets a pointer to the UI handler used in the scene.
	 *
	 * @return A pointer to the UI handler used in the scene.
	 */
	UIHandler* getUIHandle() { return &m_UIHandler; }

	/**
	 * @brief Gets a pointer to the World used in the scene.
	 *
	 * @return A pointer to the world used in the scene.
	 */
	World* getWorld() { return &m_World; }

	/**
	 * @brief Gets a pointer to the descriptor pool used in the scene.
	 *
	 * @return A pointer to the descriptor pool used in the scene.
	 */
	DescriptorPool* getDescriptorPool() { return &m_DescriptorsPool; }

	inline Ref<DescriptorSetLayout> getDefaultDSL() { return m_GameObjectDSL; }

	~Scene() = default;

	friend class Application;
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

	/**
	 * @brief Records the command buffer for rendering the scene.
	 *
	 * @param target The frame context to record the command buffer.
	 */
	void recordCommandBuffer(FrameContext& target);

	/**
	 * @brief Updates the uniforms in the frame context.
	 *
	 * @param target The frame context to update the uniforms.
	 */
	void updateUniforms(FrameContext& target);

	/**
	 * @brief Marks the frame as modified.
	 */
	void setModified();

	/**
	 * @brief Performs scene cleanup and releases resources.
	 */
	void cleanup();
};

}
