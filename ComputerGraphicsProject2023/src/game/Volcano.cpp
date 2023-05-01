#include "Volcano.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

void Volcano::update(float dt)
{
	float x = Input::getAxis("MOVE_LEFT", "MOVE_RIGHT") * c_Speed * dt;
	float z = Input::getAxis("MOVE_DOWN", "MOVE_UP") * c_Speed * dt;

	glm::vec3 movement(x, 0.0f, z);
	if (glm::length(movement) > 1.f) {
		movement = glm::normalize(movement);
	}

	m_GameObject->translate(movement);

	if (Input::isKeyPressed(GLFW_KEY_H))
	{
		m_GameObject->setScale(m_GameObject->getScale() + glm::vec3(c_Speed) * dt);
	}
}

} // namespace game