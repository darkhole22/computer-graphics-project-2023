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
	if (!m_Invincible)
	{
		auto collidingObjects = Application::getScene()->getCollidingObjects(transform, "ENEMY");
		if (!collidingObjects.empty())
		{
			m_HP = std::max(int(m_HP - collidingObjects.size()), 0);
			EventBus::emit(HealthUpdated{ m_HP, m_MaxHP });

			m_Invincible = true;
			auto invincibilityTween = Application::getScene()->makeTween();
			invincibilityTween->addIntervalTweener(m_InvincibilityDuration);
			invincibilityTween->addCallbackTweener([this]() {
				m_Invincible = false;
			});
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