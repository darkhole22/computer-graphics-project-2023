#include "Rock.h"

#include "vulture/core/Application.h"
#include "vulture/util/Random.h"

namespace game {

using namespace vulture;

Rock::Rock()
{
	m_Scene = Application::getScene();
	m_GameObject = makeRef<GameObject>("rock", "rock", DEFAULT_EMISSION_TEXTURE_NAME, "rock");
	m_GameObject->transform->setScale(0.05f);
	m_GameObject->transform->rotate(0.0f, Random::next(0.0f, glm::radians(360.0f)), 0.0f);

	m_Scene->addObject(m_GameObject);
}

void Rock::setPosition(glm::vec3 position)
{
	m_GameObject->transform->setPosition(position);
}

Rock::~Rock()
{
	m_Scene->removeObject(m_GameObject);
}

}