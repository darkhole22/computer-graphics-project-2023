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


	EventBus::addCallback([this](EnemyDied event) { onEnemyKilled(event); });

	/**********
	 * FIRING *
	 **********/
	m_FiringTween = Application::getScene()->makeTween();
	m_FiringTween->loop();
	m_FiringTween->addCallbackTweener([this]() {
		auto bullet = m_BulletFactory->get();

		bullet->setup(transform.getPosition(), m_Camera->direction);

		EventBus::emit(AmmoUpdated{ 10, 20 });
	});
	m_FiringTween->addIntervalTweener(m_Stats.fireCooldown);
	m_FiringTween->addCallbackTweener([this]() {
		if (!Input::isActionPressed("FIRE"))
		{
			m_FiringTween->pause();
		}
	});
	m_FiringTween->pause();

	/**********
	 * HITBOX *
	 **********/
	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.0f, c_CameraHeight));

	m_Hitbox->layerMask = PLAYER_MASK;
	m_Hitbox->collisionMask = ENEMY_MASK;

	m_Hitbox->transform = transform;
	scene->addHitbox(m_Hitbox);

	m_Hitbox->addCallback([this](const HitBoxEntered& event) { onHitBoxEntered(event); });

	EventBus::emit(HealthUpdated{ m_Stats.hp, m_Stats.maxHp });
}

void Player::update(f32 dt)
{
	if (m_Stats.dashesLeft > 0 &&
		m_Stats.dashSpeed == 1.0f &&
		Input::isActionJustPressed("DASH"))
	{
		m_Stats.dashSpeed = m_Stats.maxDashSpeed;
		m_Invincible = true;

		auto tween = Application::getScene()->makeTween();
		tween->addValueTweener(&m_Stats.dashSpeed, 1.0f, m_Stats.dashDuration);
		tween->addCallbackTweener([this]() { m_Invincible = false; });

		Application::getScene()->makeTimer(m_Stats.dashCooldown)->addCallback([this](TimerTimeoutEvent e) {
			m_Stats.dashesLeft++;
		});
		m_Stats.dashesLeft--;
	}

	auto rotation = Input::getVector("LOOK_LEFT", "LOOK_RIGHT", "LOOK_DOWN", "LOOK_UP", false)
		* c_RotSpeed * dt;

	auto movement = Input::getVector("MOVE_LEFT", "MOVE_RIGHT", "MOVE_DOWN", "MOVE_UP")
		* c_Speed * m_Stats.dashSpeed * dt ;

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
		EventBus::emit(GodmodeToggled{ m_Godmode });
	}

	m_BulletFactory->update(dt);
}

void Player::reset()
{
	// TODO delete transform; or maybe use Ref
	transform = Transform();
	m_Camera->reset();
	m_Camera->position = transform.getPosition() + glm::vec3(0.0f, c_CameraHeight, 0.0f);

	m_Stats = PlayerStats{};
	EventBus::emit(HealthUpdated{ m_Stats.hp, m_Stats.maxHp });

	m_BulletFactory->reset();
}

void Player::onHitBoxEntered(const HitBoxEntered &e)
{
	if (m_Invincible || m_Godmode) return;

	u32 damage = e.data != nullptr ? *reinterpret_cast<u32*>(e.data) : 1;

	m_Stats.hp = std::max<i32>(m_Stats.hp - damage, 0);
	EventBus::emit(HealthUpdated{ m_Stats.hp, m_Stats.maxHp });

	m_Invincible = true;
	auto invincibilityTween = Application::getScene()->makeTween();
	invincibilityTween->addIntervalTweener(m_InvincibilityDuration);
	invincibilityTween->addCallbackTweener([this]() {
		m_Invincible = false;
	});
}

void Player::onEnemyKilled(const EnemyDied& event)
{
	m_Stats.exp++;

	if (m_Stats.exp >= 10)
	{
		VUDEBUG("Level Up!");

		m_Stats.level += m_Stats.exp / 10;
		m_Stats.exp %= 10;

		m_Stats.dashesLeft++;
	}

}

} // namespace game