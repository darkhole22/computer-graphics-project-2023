#include "Player.h"

using namespace vulture;

namespace game {

const String& Bullet::s_ModelName = "character";
const String& Bullet::s_TextureName = "character";

void Player::update(f32 dt)
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
	m_Camera->position = transform.getPosition() + glm::vec3(0.0f, c_CameraHeight, 0.0f);

	// std::cout << glm::to_string(transform.getPosition()) << "; " << glm::to_string(m_Camera->position) << std::endl;

	if (Input::isActionJustPressed("FIRE"))
	{
		auto bullet = m_BulletFactory->get();

		bullet->setup(transform.getPosition(), m_Camera->direction);
	}

	m_BulletFactory->update(dt);
}

} // namespace game