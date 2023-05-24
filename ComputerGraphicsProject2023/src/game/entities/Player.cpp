#include "Player.h"

using namespace vulture;

namespace game {

Player::Player()
{
	m_Camera = Application::getScene()->getCamera();
	m_Camera->position = transform.getPosition() + glm::vec3(0.0f, c_CameraHeight, 0.0f);

	m_BulletFactory = makeRef<Factory<Bullet>>(40);

	EventBus::emit(HealthUpdated{m_HP, m_MaxHP});
}

void Player::update(f32 dt)
{
	static float invincibility = 0.0f;
	invincibility += dt;

	if (invincibility >= 1.0f) {
		auto collidingObjects = Application::getScene()->getCollidingObjects(transform, "ENEMY");
		if (!collidingObjects.empty()) {
			m_HP = std::max(int(m_HP - collidingObjects.size()), 0);
			EventBus::emit(HealthUpdated{m_HP, m_MaxHP});

			invincibility = 0.0f;
		}
	}

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
		bullet->m_GameObject->tag = "PLAYER_BULLET";

		bullet->setup(transform.getPosition(), m_Camera->direction);

		EventBus::emit(AmmoUpdated{10, 20});
	}

	m_BulletFactory->update(dt);
}

} // namespace game