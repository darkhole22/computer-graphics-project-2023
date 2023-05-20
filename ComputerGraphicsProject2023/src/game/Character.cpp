#include "Character.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

void Character::update(float dt)
{
	auto rotation = Input::getVector("ROTATE_LEFT", "ROTATE_RIGHT", "ROTATE_DOWN", "ROTATE_UP");
	auto movement = Input::getVector("MOVE_LEFT", "MOVE_RIGHT", "MOVE_DOWN", "MOVE_UP");
	auto roll = Input::getAxis("ROLL_LEFT", "ROLL_RIGHT");

	m_Camera->rotate(glm::vec3(-rotation.x, rotation.y, 0.0f) * c_RotSpeed * dt);
	m_Camera->addRoll(roll * c_RotSpeed * dt);

	m_Camera->translate(glm::vec3(movement.x, 0.0f, movement.y) * c_Speed * dt);
}

} // namespace game