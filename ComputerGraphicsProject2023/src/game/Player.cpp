#include "Player.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

void Player::update(float dt)
{
	auto rotation = Input::getVector("ROTATE_LEFT", "ROTATE_RIGHT", "ROTATE_DOWN", "ROTATE_UP")
			* c_RotSpeed * dt;
	auto movement = Input::getVector("MOVE_LEFT", "MOVE_RIGHT", "MOVE_DOWN", "MOVE_UP")
			* c_Speed * dt;

	// Move the player
	transform.rotate(0.0f, -rotation.x, 0.0f);
	transform.translate(movement.y, 0.0f, movement.x);

	// Move the camera
	m_Camera->rotate(-rotation.x, rotation.y, 0.0f);
	m_Camera->translate(movement.x, 0.0f, movement.y);

	std::cout << glm::to_string(transform.getPosition()) << " ; " << glm::to_string(m_Camera->position) << std::endl;
}

} // namespace game