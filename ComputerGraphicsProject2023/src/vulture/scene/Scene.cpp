#include "Scene.h"

// #define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"

namespace vulture {

RenderableObject::RenderableObject(Ref<Model> model, Ref<DescriptorSet> descriptorSet) :
	m_Model(model), m_DescriptorSet(descriptorSet)
{}

SceneObjectList::SceneObjectList(const String& vertexShader,
								 const String& fragmentShader, const std::vector<DescriptorSetLayout*>& descriptorSetLayouts,
								 PipelineAdvancedConfig config) :
	m_Pipeline(new Pipeline(Renderer::getRenderPass(), vertexShader, fragmentShader, descriptorSetLayouts, Renderer::getVertexLayout(), config))
{}

void SceneObjectList::addObject(ObjectHandle handle, const RenderableObject& obj)
{
	m_Objects.insert({ handle, obj });
}

void SceneObjectList::removeObject(ObjectHandle handle)
{
	auto it = m_Objects.find(handle);
	if (it == m_Objects.end()) return;
	m_Objects.erase(it);
}

Scene::Scene() :
	m_DescriptorsPool(Renderer::makeDescriptorPool()),
	m_Camera(m_DescriptorsPool), m_Skybox(m_DescriptorsPool), m_UIHandler(m_DescriptorsPool), m_World(m_DescriptorsPool)
{
	// Create the default Phong GameObject DSL.
	m_GameObjectDSL = Ref<DescriptorSetLayout>(new DescriptorSetLayout());
	m_GameObjectDSL->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_GameObjectDSL->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_GameObjectDSL->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_GameObjectDSL->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_GameObjectDSL->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_GameObjectDSL->create();

	// Create default Phong pipeline.
	m_GameObjectPipeline = makePipeline("res/shaders/Phong_vert.spv", "res/shaders/Phong_frag.spv", m_GameObjectDSL);

	setModified();

	m_Skybox.addCallback([this](const SkyboxRecreated& event) {
		setModified();
	});

	m_UIHandler.addCallback([this](const UIModified& event) {
		setModified();
	});
}

template<class T>
void stepUtil(std::unordered_set<Ref<T>>& set, f32 dt, bool& looping)
{
	looping = true;
	for (auto it = set.begin(); it != set.end();)
	{
		T* util = it->get();
		util->step(dt);
		if (!util->isRunning())
		{
			util->stop();
			VUTRACE("\"%s\" removed from the scene!", typeid(T).name());
			it = set.erase(it);
		}
		else
			++it;
	}
	looping = false;
}

void Scene::render(FrameContext target, f32 dt)
{
	// If the swap chain has been recreated, set frame as modified.
	if (target.updated())
	{
		setModified();
	}

	// If the number of frames in flight has changed, set frame as modified.
	// This happens only in very exceptional cases.
	auto [index, count] = target.getFrameInfo();
	if (m_FrameModified.size() < count)
	{
		m_FrameModified.resize(count);
		m_DescriptorsPool.setFrameCount(count);
		setModified();
	}

	// Update all GameObjects in the scene.
	for (const auto& it : m_GameObjects)
	{
		it.second->update(dt);
	}

	// Update the camera
	auto [width, height] = target.getExtent();
	f32 aspectRatio = static_cast<f32>(width) / height;
	m_Camera.m_AspectRatio = aspectRatio;
	m_Camera.update(dt);

	// Update collision engine
	m_CollisionEngine.update(dt);

	// Tick tweens and timers
	if (!m_Paused)
	{
		stepUtil(m_Tweens, dt, m_TweenLoopFlag);
		stepUtil(m_Timers, dt, m_TimersLoopFlag);
	}
	// Update UI
	// UI has to be modified after everything else to ensure
	// correct text centering and repositioning.
	m_UIHandler.m_ScreenUniform->width = static_cast<f32>(width);
	m_UIHandler.m_ScreenUniform->height = static_cast<f32>(height);
	m_UIHandler.update(dt);

	if (m_FrameModified[index])
	{
		recordCommandBuffer(target);
		m_FrameModified[index] = false;
	}

	// Update the uniform buffers of every renderable object in the scene
	updateUniforms(target);

	// Call deferred functions
	// Moving the functions in a local variable to avoid insertion during the loop.
	std::vector<std::function<void()>> deferredFunctions = std::move(m_DeferredFunctions);
	for (auto& fun : deferredFunctions)
	{
		fun();
	}

	// Frame Context goes out of scope here and is destroyed, causing the SwapChain to submit.
}

PipelineHandle Scene::makePipeline(const String& vertexShader, const String& fragmentShader, Ref<DescriptorSetLayout> descriptorSetLayout, PipelineAdvancedConfig config)
{
	PipelineHandle handle = m_NextPipelineHandle++;

	std::vector<DescriptorSetLayout*> layouts{};
	layouts.push_back(descriptorSetLayout.get());
	layouts.push_back(m_Camera.getDescriptorSetLayout());
	layouts.push_back(m_World.getDescriptorSetLayout());

	m_ObjectLists.insert({ handle, SceneObjectList(vertexShader, fragmentShader, layouts, config) });

	return handle;
}

ObjectHandle Scene::addObject(PipelineHandle pipeline, Ref<Model> model, Ref<DescriptorSet> descriptorSet)
{
	auto p = m_ObjectLists.find(pipeline);
	if(p != m_ObjectLists.end())
	{
		auto handle = GameObject::s_NextHandle++;

		p->second.addObject(handle, RenderableObject(model, descriptorSet));

		setModified();
		return handle;
	}
	VUWARN("Trying to add an object to an invalid pipeline (%li)!", pipeline);
	return 0;
}

void Scene::removeObject(PipelineHandle pipeline, ObjectHandle obj)
{
	auto p = m_ObjectLists.find(pipeline);
	if (p != m_ObjectLists.end())
	{
		p->second.removeObject(obj);
		setModified();
	}
}

void Scene::addObject(Ref<GameObject> obj)
{
	addObject(obj, m_GameObjectPipeline);
}

void Scene::addObject(Ref<GameObject> obj, PipelineHandle pipelineHandle)
{
	auto p = m_ObjectLists.find(pipelineHandle);
	if (p == m_ObjectLists.end())
	{
		VUWARN("Trying to add an object to an invalid pipeline (%li)!", pipelineHandle);
		return;
	}

	p->second.addObject(
			obj->m_Handle,
			RenderableObject(
					obj->m_Model,
					m_DescriptorsPool.getDescriptorSet(
							m_GameObjectDSL,
							{obj->m_ModelUniform, *obj->m_TextureSampler, *obj->m_EmissionTextureSampler, *obj->m_RoughnessTextureSampler, obj->m_ObjectUniform }
					)
			)
	);
	m_GameObjects[obj->m_Handle] = obj;

	setModified();
}

void Scene::removeObject(Ref<GameObject> obj)
{
	removeObject(obj, m_GameObjectPipeline);
}

void Scene::removeObject(Ref<GameObject> obj, PipelineHandle pipelineHandle)
{
	auto it = m_GameObjects.find(obj->m_Handle);
	if (it == m_GameObjects.end()) return;
	m_GameObjects.erase(it);

	auto p = m_ObjectLists.find(pipelineHandle);
	if (p == m_ObjectLists.end())
	{
		VUWARN("Trying to remove an object from an invalid pipeline (%li)!", pipelineHandle);
		return;
	}

	p->second.removeObject(obj->m_Handle);
	setModified();
}

void Scene::setSkybox(const String& name)
{
	m_Skybox.set(name);
}

Ref<Tween> Scene::makeTween()
{
	auto tween = makeRef<Tween>();
	if (!m_TweenLoopFlag)
		m_Tweens.insert(tween);
	else
		callLater([this, tween]() {m_Tweens.insert(tween); });
	return tween;
}

Ref<Timer> Scene::makeTimer(f32 waitTime, bool oneShot)
{
	auto timer = makeRef<Timer>(waitTime, oneShot);
	if (!m_TimersLoopFlag)
		m_Timers.insert(timer);
	else
		callLater([this, timer]() {m_Timers.insert(timer); });
	return timer;
}

void Scene::callLater(std::function<void()> fun)
{
	m_DeferredFunctions.push_back(fun);
}

/*
 * The command buffer has to be recorded everytime one of the following happens:
 * - The number of objects in the scene changes
 * - The window size changes
 * - Other rendering-related details change
 */
void Scene::recordCommandBuffer(FrameContext& target)
{
	target.beginCommandRecording();

	m_Skybox.recordCommandBuffer(target);

	for (auto& [pipelineHandle, objectList] : m_ObjectLists)
	{
		auto& pipeline = objectList.getPipeline();
		target.bindPipeline(pipeline);

		target.bindDescriptorSet(pipeline, m_Camera.getDescriptorSet(), 1);
		target.bindDescriptorSet(pipeline, m_World.getDescriptorSet(), 2);

		for (auto& [objectHandle, object] : objectList)
		{
			target.bindDescriptorSet(pipeline, object.getDescriptorSet(), 0);

			target.drawModel(object.getModel());
		}
	}

	m_UIHandler.recordCommandBuffer(target);

	target.endCommandRecording();
}

void Scene::updateUniforms(FrameContext& target)
{
	auto [index, count] = target.getFrameInfo();

	m_Camera.map(index);

	m_Skybox.updateUniforms(target, m_Camera);
	m_World.updateUniforms(target, m_Camera);

	for (auto& [pipelineHandle, objectList] : m_ObjectLists)
	{
		for (auto& [objectHandle, object] : objectList)
		{
			object.getDescriptorSet().map(index);
		}
	}

	m_UIHandler.updateUniforms(target);
}

void Scene::setModified()
{
	for (size_t i = 0; i < m_FrameModified.size(); i++)
	{
		m_FrameModified[i] = true;
	}
}

void Scene::cleanup()
{
	m_Timers.clear();
	m_Tweens.clear();
}

} // namespace vulture
