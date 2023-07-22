#include "Tree.h"

#include "vulture/core/Application.h"

namespace game {

Tree::Tree()
	: m_Scene(Application::getScene())
{
	PipelineAdvancedConfig config{};
	config.cullMode = VK_CULL_MODE_NONE;

	m_LeavesPipelineHandle = m_Scene->makePipeline(
			"res/shaders/Phong_vert.spv", "res/shaders/Phong_frag.spv",
			m_Scene->getDefaultDSL(), config);

	m_TrunkGameObject = makeRef<GameObject>("tree-trunk", "tree-trunk", DEFAULT_EMISSION_TEXTURE_NAME, "rough");
	m_LeavesGameObject = makeRef<GameObject>("tree-leaves", "tree-leaves", DEFAULT_EMISSION_TEXTURE_NAME, "rough");

	m_TrunkGameObject->transform->setScale(0.05f);
	m_LeavesGameObject->transform->setScale(0.05f);

	m_Scene->addObject(m_TrunkGameObject);
	m_Scene->addObject(m_LeavesGameObject, m_LeavesPipelineHandle);
}

void Tree::setPosition(glm::vec3 position)
{
	m_TrunkGameObject->transform->setPosition(position);
	m_LeavesGameObject->transform->setPosition(position);
}

Tree::~Tree()
{
	m_Scene->removeObject(m_TrunkGameObject);
	m_Scene->removeObject(m_LeavesGameObject, m_LeavesPipelineHandle);
}

} // namespace game