#include "Player.h"

#include "vulture/core/Logger.h"

using namespace vulture;

namespace game {

Player::Player()
{
	auto scene = Application::getScene();
	m_Camera = scene->getCamera();
	m_Camera->setFarPlane(200);
	m_Camera->position = transform.getPosition() + glm::vec3(0.0f, c_CameraHeight, 0.0f);

	m_BulletFactory = makeRef<Factory<Bullet>>(40);

	m_FiringTween = Application::getScene()->makeTween();
	m_FiringTween->loop();
	m_FiringTween->addCallbackTweener([this]() {
		auto bullet = m_BulletFactory->get();
		bullet->m_GameObject->tag = "PLAYER_BULLET";

		bullet->setup(transform.getPosition(), m_Camera->direction);

		EventBus::emit(AmmoUpdated{ 10, 20 });
	});
	m_FiringTween->addIntervalTweener(c_FireRatio);
	m_FiringTween->addCallbackTweener([this]() {
		if (!Input::isActionPressed("FIRE"))
		{
			m_FiringTween->pause();
		}
	});
	m_FiringTween->pause();

	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.0f, c_CameraHeight));

	m_Hitbox->layerMask = PLAYER_MASK;
	m_Hitbox->collisionMask = ENEMY_MASK;

	m_Hitbox->transform = transform;
	scene->addHitbox(m_Hitbox);

	m_Hitbox->addCallback([this](const HitBoxEntered& e) {
		if (m_Invincible || m_Godmode) return;

		m_HP = std::max(static_cast<i32>(m_HP) - 1, 0);
		EventBus::emit(HealthUpdated{ m_HP, m_MaxHP });

		m_Invincible = true;
		auto invincibilityTween = Application::getScene()->makeTween();
		invincibilityTween->addIntervalTweener(m_InvincibilityDuration);
		invincibilityTween->addCallbackTweener([this]() {
			m_Invincible = false;
		});
	});

	m_BulletFactory = makeRef<Factory<Bullet>>(40);

	EventBus::emit(HealthUpdated{ m_HP, m_MaxHP });
}

void Player::update(f32 dt)
{
	auto rotation = Input::getVector("LOOK_LEFT", "LOOK_RIGHT", "LOOK_DOWN", "LOOK_UP", false)
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
		m_FiringTween->play();
	}

	if (Input::isActionJustPressed("TOGGLE_GODMODE"))
	{
		m_Godmode = !m_Godmode;
	}

	m_BulletFactory->update(dt);
}

} // namespace game