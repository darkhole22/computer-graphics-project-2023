#include "Scene.h"

// #define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"

namespace vulture {

RenderableObject::RenderableObject(Ref<Model> model, Ref<DescriptorSet> descriptorSet) :
	m_Model(model), m_DescriptorSet(descriptorSet)
{}

SceneObjectList::SceneObjectList(const String& vertexShader,
								 const String& fragmentShader, const std::vector<DescriptorSetLayout*>& descriptorSetLayouts) :
	m_Pipeline(new Pipeline(Renderer::getRenderPass(), vertexShader, fragmentShader, descriptorSetLayouts, Renderer::getVertexLayout()))
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
	m_GameObjectDSL->create();

	// Create default Phong pipeline.
	// TODO Consider removing hardcoded values
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
void stepUtil(std::unordered_set<Ref<T>>& set, f32 dt)
{
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
}

void Scene::render(FrameContext target, float dt)
{
	if (target.updated())
	{
		setModified();
	}

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

	auto [width, height] = target.getExtent();
	float aspectRatio = static_cast<float>(width) / height;
	m_Camera.m_AspectRatio = aspectRatio;
	m_Camera.update(dt);

	m_UIHandler.m_ScreenUniform->width = static_cast<float>(width);
	m_UIHandler.m_ScreenUniform->height = static_cast<float>(height);
	m_UIHandler.update(dt);

	stepUtil(m_Tweens, dt);
	stepUtil(m_Timerss, dt);

	m_CollisionEngine.update(dt);

	if (m_FrameModified[index])
	{
		recordCommandBuffer(target);
		m_FrameModified[index] = false;
	}

	updateUniforms(target);
}

PipelineHandle Scene::makePipeline(const String& vertexShader, const String& fragmentShader, Ref<DescriptorSetLayout> descriptorSetLayout)
{
	PipelineHandle handle = m_NextPipelineHandle++;

	std::vector<DescriptorSetLayout*> layouts{};
	layouts.push_back(descriptorSetLayout.get());
	layouts.push_back(m_Camera.getDescriptorSetLayout());
	layouts.push_back(m_World.getDescriptorSetLayout());

	m_ObjectLists.insert({ handle, SceneObjectList(vertexShader, fragmentShader, layouts) });

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
	auto& p = m_ObjectLists.at(m_GameObjectPipeline);

	p.addObject(obj->m_Handle, RenderableObject(obj->m_Model, m_DescriptorsPool.getDescriptorSet(m_GameObjectDSL, { obj->m_Uniform, *obj->m_TextureSampler })));
	m_GameObjects[obj->m_Handle] = obj;

	setModified();
}

void Scene::removeObject(Ref<GameObject> obj)
{
	auto it = m_GameObjects.find(obj->m_Handle);
	if (it == m_GameObjects.end()) return;
	m_GameObjects.erase(it);

	auto& p = m_ObjectLists.at(m_GameObjectPipeline);
	p.removeObject(obj->m_Handle);

	setModified();
}

void Scene::setSkybox(const String& name)
{
	m_Skybox.set(name);
}

Ref<Tween> Scene::makeTween()
{
	auto tween = makeRef<Tween>();
	m_Tweens.insert(tween);
	return tween;
}

Ref<Timer> Scene::makeTimer(f32 waitTimer, bool oneShot)
{
	auto tween = makeRef<Timer>(waitTimer, oneShot);
	m_Timerss.insert(tween);
	return tween;
}

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

} // namespace vulture
