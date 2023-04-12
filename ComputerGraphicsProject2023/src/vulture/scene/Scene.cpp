#include "Scene.h"

#include <iostream>

namespace vulture {

RenderableObject::RenderableObject(Ref<Model> model, Ref<DescriptorSet> descriptorSet) :
	m_Model(model), m_DescriptorSet(descriptorSet)
{

}

SceneObjectList::SceneObjectList(const Renderer& renderer, const std::string& vertexShader, 
	const std::string& fragmentShader, const std::vector<DescriptorSetLayout*>& descriptorSetLayouts) :
	m_Pipeline(new Pipeline(renderer.getRenderPass(), vertexShader, fragmentShader, descriptorSetLayouts, Renderer::getVertexLayout()))
{
}

ObjectHandle SceneObjectList::addObject(RenderableObject obj)
{
	ObjectHandle handle = m_NextObjectHandle++; // Replace with a pseudo random number generator

	m_Objects.insert({ handle, obj });

	return handle;
}

void SceneObjectList::removeObject(ObjectHandle handle)
{
	auto& it = m_Objects.find(handle);
	m_Objects.erase(it);
}

Scene::Scene(const Renderer& renderer) :
	m_Renderer(&renderer), m_DescriptorsPool(renderer.makeDescriptorPool()), 
	m_Camera(renderer, m_DescriptorsPool), m_UIHandler(renderer, m_DescriptorsPool)
{
	setModified();

	m_UIHandler.addCallback([this](const UIModified& event) {
		setModified();
	});
}

void Scene::render(RenderTarget target, float dt)
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

	auto& [width, height] = target.getExtent();
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

	auto& [width, height] = target.getExtent();
	m_Camera.m_AspectRatio = static_cast<float>(width) / height;
	m_Camera.update(dt);

	updateUniforms(target);
}

PipelineHandle Scene::makePipeline(const std::string& vertexShader, const std::string& fragmentShader, Ref<DescriptorSetLayout> descriptorSetLayout)
{
	PipelineHandle handle = m_NextPipelineHandle++; // Consider using a pseudo number generator

	std::vector<DescriptorSetLayout*> layouts{};
	layouts.push_back(descriptorSetLayout.get());
	layouts.push_back(m_Camera.getDescriptorSetLayout());

	m_ObjectLists.insert({ handle, SceneObjectList(*m_Renderer, vertexShader, fragmentShader, layouts) });

	return handle;
}

ObjectHandle Scene::addObject(PipelineHandle pipeline, Ref<Model> model, Ref<DescriptorSetLayout> layout, const std::vector<DescriptorWrite>& descriptorWrites)
{
	auto& p = m_ObjectLists.at(pipeline);

	auto handle = p.addObject(RenderableObject(model, m_DescriptorsPool.getDescriptorSet(*layout.get(), descriptorWrites)));

	setModified();
	return handle;
}

void Scene::recordCommandBuffer(RenderTarget& target)
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

void Scene::updateUniforms(RenderTarget& target)
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
