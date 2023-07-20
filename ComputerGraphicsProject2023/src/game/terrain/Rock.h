#pragma once

#include "vulture/scene/Scene.h"
#include "vulture/scene/GameObject.h"

namespace game {

using namespace vulture;

class Rock
{
public:
	Rock();

	void setPosition(glm::vec3 position);

	~Rock();
private:
	Scene* m_Scene;
	Ref<GameObject> m_GameObject;
};

} // namespace game