#include "Tree.h"

#include "vulture/core/Application.h"

namespace game {


Tree::Tree()
{
	trunkGameObject = makeRef<GameObject>("tree-trunk", "tree-trunk");
	leavesGameObject = makeRef<GameObject>("tree-leaves", "tree-leaves");

	trunkGameObject->transform->setScale(0.05f);
	leavesGameObject->transform->setScale(0.05f);

	Application::getScene()->addObject(trunkGameObject);
	Application::getScene()->addObject(leavesGameObject);
}

void Tree::setPosition(glm::vec3 position)
{
	trunkGameObject->transform->setPosition(position);
	leavesGameObject->transform->setPosition(position);
}

Tree::~Tree()
{
	Application::getScene()->removeObject(trunkGameObject);
	Application::getScene()->removeObject(leavesGameObject);
}

} // namespace game