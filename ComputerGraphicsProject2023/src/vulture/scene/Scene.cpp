#include "Scene.h"

#include <iostream>

namespace vulture {

RenderableObject::RenderableObject(Ref<Model> model, Ref<DescriptorSet> descriptorSet) :
	m_Model(model), m_DescriptorSet(descriptorSet)
{
}

SceneObjectList::SceneObjectList(const String& vertexShader,
	const String& fragmentShader, const std::vector<DescriptorSetLayout*>& descriptorSetLayouts) :
	m_Pipeline(new Pipeline(Renderer::getRenderPass(), vertexShader, fragmentShader, descriptorSetLayouts, Renderer::getVertexLayout()))
{
}

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
	m_Camera(m_DescriptorsPool), m_UIHandler(m_DescriptorsPool)
{
	// Create the default Phong GameObject DSL.
	m_GameObjectDSL = Ref<DescriptorSetLayout>(new DescriptorSetLayout());
	m_GameObjectDSL->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_GameObjectDSL->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_GameObjectDSL->create();

	// Create default Phong pipeline.
	// TODO Consider removing hardcoded values
	m_GameObjectPipeline = makePipeline("res/shaders/baseVert.spv", "res/shaders/baseFrag.spv", m_GameObjectDSL);

	setModified();

	m_UIHandler.addCallback([this](const UIModified& event) {
		setModified();
	});
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

	auto [width, height] = target.getExtent();
	float aspectRatio = static_cast<float>(width) / height;
	m_Camera.m_AspectRatio = aspectRatio;
	m_Camera.update(dt);

	m_UIHandler.m_ScreenUniform->width = static_cast<float>(width);
	m_UIHandler.m_ScreenUniform->height = static_cast<float>(height);
	m_UIHandler.update(dt);

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

	std::vector<DescriptorSetLayout *> layouts{};
	layouts.push_back(descriptorSetLayout.get());
	layouts.push_back(m_Camera.getDescriptorSetLayout());

	m_ObjectLists.insert({ handle, SceneObjectList(vertexShader, fragmentShader, layouts) });

	return handle;
}

void Scene::addObject(Ref<GameObject> obj)
{
	auto& p = m_ObjectLists.at(m_GameObjectPipeline);

	p.addObject(obj->m_Handle, RenderableObject(obj->m_Model, m_DescriptorsPool.getDescriptorSet(*m_GameObjectDSL.get(), {obj->m_Uniform, *obj->m_TextureSampler})));

	setModified();
}

void Scene::removeObject(Ref<GameObject> obj)
{
	auto& p = m_ObjectLists.at(m_GameObjectPipeline);
	p.removeObject(obj->m_Handle);
}

void Scene::recordCommandBuffer(FrameContext& target)
{
	target.beginCommandRecording();

	for (auto& [pipelineHandle, objectList] : m_ObjectLists)
	{
		auto& pipeline = objectList.getPipeline();
		target.bindPipeline(pipeline);

		target.bindDescriptorSet(pipeline, m_Camera.getDescriptorSet(), 1);

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
