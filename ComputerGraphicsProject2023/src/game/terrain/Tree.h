#pragma once

#include "vulture/scene/GameObject.h"

namespace game {

using namespace vulture;

class Tree
{
public:
	Ref<GameObject> trunkGameObject;
	Ref<GameObject> leavesGameObject;

	explicit Tree();

	void setPosition(glm::vec3 position);

	~Tree();
};

} // namespace game