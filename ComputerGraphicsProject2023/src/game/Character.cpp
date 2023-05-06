#include "Character.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

void Character::update(float dt)
{
	auto movement = Input::getVector("MOVE_LEFT", "MOVE_RIGHT", "MOVE_UP", "MOVE_DOWN") * c_Speed * dt;

	m_GameObject->translate(movement.x, 0.0f, movement.y);

	if (Input::isKeyPressed(GLFW_KEY_H))
	{
		m_GameObject->setScale(m_GameObject->getScale() + glm::vec3(c_Speed) * dt);
	}

	/*
	static float rot = 0.0f;
	m_GameObject->setRotation(0.0f, rot, 0.0f);
	rot += c_RotSpeed * dt;
	 */

	// m_GameObject->rotate(glm::vec3(0.0f, 1.0f, 0.0f), c_RotSpeed * dt);

	m_GameObject->rotate(0.0f, c_RotSpeed * dt, 0.0f);
}

} // namespace game