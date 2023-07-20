#pragma once

#include "vulture/scene/Scene.h"
#include "vulture/scene/GameObject.h"


namespace game {

using namespace vulture;

class Tree
{
public:
	Tree();

	void setPosition(glm::vec3 position);

	~Tree();
private:
	Scene* m_Scene;
	PipelineHandle m_LeavesPipelineHandle;

	Ref<GameObject> m_TrunkGameObject;
	Ref<GameObject> m_LeavesGameObject;
};

} // namespace game