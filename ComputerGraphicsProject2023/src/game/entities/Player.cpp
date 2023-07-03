#include "Player.h"

#include "vulture/event/Event.h"
#include "vulture/core/Input.h"
#include "vulture/core/Application.h"
#include "vulture/util/Random.h"
#include "vulture/core/Logger.h"

#include "game/EventBus.h"
#include "game/entities/CollisionMask.h"
#include "game/entities/powerup/HealthPack.h"
#include "game/entities/powerup/DoubleScore.h"
#include "game/entities/powerup/Bomb.h"

#include <utility>
#include <algorithm>

namespace game {

using namespace vulture;

Player::Player(Ref<Terrain> terrain) :
	m_Terrain(terrain), m_Transform(makeRef<Transform>()),
	m_GunAudio("shot"), m_DamageAudio("hurt"),
	m_ExplosionFactory(1)
{
	auto scene = Application::getScene();
	m_Camera = scene->getCamera();
	m_Camera->setFarPlane(200);

	EventBus::addCallback([this](EnemyDied event) { onEnemyKilled(event); });

	/**********
	 * FIRING *
	 **********/
	updateFiringTween();

	// Bobbing tween
	scene->makeTween()->loop()->addMethodTweener<f32>([this](f32 val) {
		m_BobbingHeight = -0.4f + 0.1f * std::sin(val);
	}, 0.0f, glm::radians(360.0f), 1.5f);

	/**********
	 * HITBOX *
	 **********/
	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.0f, m_CameraHeight));
	m_Hitbox->transform = m_Transform;

	m_Hitbox->layerMask = PLAYER_MASK;
	m_Hitbox->collisionMask = ENEMY_MASK | EXPLOSION_MASK;

	scene->addHitbox(m_Hitbox);

	m_Hitbox->addCallback([this](const HitBoxEntered& event) { onHitBoxEntered(event); });

	/*************
	 * POWER UPS *
	 *************/
	m_PowerUpHitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.5f, m_CameraHeight));
	m_PowerUpHitbox->transform = m_Transform;

	m_PowerUpHitbox->layerMask = PLAYER_MASK;
	m_PowerUpHitbox->collisionMask = POWER_UP_MASK;

	scene->addHitbox(m_PowerUpHitbox);

	m_PowerUpHitbox->addCallback([this](const HitBoxEntered& event) { onPowerUpEntered(event); });

	EventBus::addCallback([this](const ExplosionStarted&) { m_CanSpawnExplosion = false; });
	EventBus::addCallback([this](const ExplosionFinished&) { m_CanSpawnExplosion = true; });

	/*************
	 * FACTORIES *
	 *************/
	m_BulletFactory = makeRef<Factory<Bullet>>(10, glm::mat4(1.0f), 1);
	reset();
	m_Movement = makeRef<MovementComponent>(m_Transform);

	/********************
	 * INITIAL POSITION *
	 ********************/
	auto pos = m_Transform->getPosition();
	m_Transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) +
							 m_Terrain->isWater(pos.x, pos.z) * m_BobbingHeight, pos.z);

	m_Camera->position = m_Transform->getPosition() + glm::vec3(0.0f, m_CameraHeight, 0.0f);
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
			EventBus::emit(DashesUpdated{ m_Stats.dashesLeft, m_Stats.maxDashes });
		});
		m_Stats.dashesLeft--;
		EventBus::emit(DashesUpdated{ m_Stats.dashesLeft, m_Stats.maxDashes });
	}

	auto rotation = Input::getVector("LOOK_LEFT", "LOOK_RIGHT", "LOOK_DOWN", "LOOK_UP", false)
		* c_RotSpeed * dt;

	auto movement = Input::getVector("MOVE_DOWN", "MOVE_UP", "MOVE_LEFT", "MOVE_RIGHT")
		* c_Speed * m_Stats.dashSpeed * dt;

	// Move the player
	m_Transform->rotate(0.0f, -rotation.x, 0.0f);
	m_Movement->move(movement.x, 0.0f, movement.y);

	auto pos = m_Transform->getPosition();
	auto slope = m_Terrain->getSlopeAt(pos.x, pos.z);
	if (glm::length(slope) > c_MaxSlope)
	{
		m_Transform->translate(-glm::vec3(slope.x, 0, slope.y) * c_SlopeSpeed * dt);
	}
	pos = m_Transform->getPosition();
	m_Transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) +
							 m_Terrain->isWater(pos.x, pos.z) * m_BobbingHeight, pos.z);

	// Move the camera
	m_Camera->rotate(-rotation.x, rotation.y, 0.0f);
	m_Camera->position = m_Transform->getPosition() + glm::vec3(0.0f, m_CameraHeight, 0.0f);

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
	m_ExplosionFactory.update(dt);
}

void Player::reset()
{
	*m_Transform = Transform();

	m_Camera->reset();
	m_Camera->position = m_Transform->getPosition() + glm::vec3(0.0f, m_CameraHeight, 0.0f);

	m_Stats = PlayerStats{};
	EventBus::emit(HealthUpdated{ m_Stats.hp, m_Stats.maxHp });
	EventBus::emit(DashesUpdated{ m_Stats.dashesLeft, m_Stats.maxDashes });

	m_BulletFactory->reset();
	m_ExplosionFactory.reset();
}

Player::~Player()
{
	m_ExplosionFactory.reset();
}

void Player::updateFiringTween()
{
	if (m_FiringTween) m_FiringTween->stop();

	m_FiringTween = Application::getScene()->makeTween();
	m_FiringTween->loop();
	m_FiringTween->addCallbackTweener([this]() {
		auto bullet = m_BulletFactory->get();

		bullet->setup(*m_Transform, m_Camera->direction, m_Stats.maxBulletHits);

		m_GunAudio.play();
		EventBus::emit(BulletShot{ m_Stats.fireCooldown });
	});
	m_FiringTween->addIntervalTweener(m_Stats.fireCooldown);
	m_FiringTween->addCallbackTweener([this]() {
		if (!Input::isActionPressed("FIRE"))
		{
			m_FiringTween->pause();
		}
	});
	if (!Input::isActionPressed("FIRE"))
	{
		m_FiringTween->pause();
	}
}

void Player::onHitBoxEntered(const HitBoxEntered& event)
{
	if (m_Invincible || m_Godmode || m_Stats.hp == 0) return;

	u32 damage = event.data != nullptr ? *reinterpret_cast<u32*>(event.data) : 1;

	m_Stats.hp = std::max<i32>(m_Stats.hp - damage, 0);
	EventBus::emit(HealthUpdated{ m_Stats.hp, m_Stats.maxHp });

	m_Invincible = true;
	auto invincibilityTween = Application::getScene()->makeTween();
	invincibilityTween->addIntervalTweener(m_InvincibilityDuration);
	invincibilityTween->addCallbackTweener([this]() {
		m_Invincible = false;
	});

	m_DamageAudio.play();
}

void Player::onEnemyKilled(const EnemyDied& event)
{
	m_Stats.exp++;

	if (m_Stats.exp >= PlayerStats::c_ExpRequired)
	{
		m_Stats.level += m_Stats.exp / PlayerStats::c_ExpRequired;
		m_Stats.exp %= PlayerStats::c_ExpRequired;

		f32 rand = Random::next();
		if (rand < 0.25f)
		{
			m_Stats.dashesLeft++;
			m_Stats.maxDashes++;

			EventBus::emit(DashesUpdated{ m_Stats.dashesLeft, m_Stats.maxDashes });
			EventBus::emit(LevelUp{ "Dash Upgraded!" });
		}
		else if (rand < 0.50f)
		{
			m_Stats.maxHp++;
			m_Stats.hp++;

			EventBus::emit(HealthUpdated{ m_Stats.hp, m_Stats.maxHp });
			EventBus::emit(LevelUp{ "Health Upgraded!" });
		}
		else if (rand < 0.75f && m_Stats.fireCooldown > 0.1f)
		{
			m_Stats.fireCooldown -= 0.1f;
			updateFiringTween();

			EventBus::emit(LevelUp{ "Fire Ratio Upgraded!" });
		}
		else
		{
			m_Stats.maxBulletHits++;
			EventBus::emit(LevelUp{ "Bullets Upgraded!" });
		}
	}
}

void Player::onPowerUpEntered(const HitBoxEntered& event)
{
	auto* powerUp = reinterpret_cast<PowerUpData*>(event.data);
	switch (powerUp->getType())
	{
	case PowerUpType::HealthUp:
	{
		if (m_Stats.hp < m_Stats.maxHp)
		{
			auto* healthPack = reinterpret_cast<HealthPackData*>(powerUp);
			m_Stats.hp = std::min<i32>(m_Stats.hp + healthPack->getHealth(), m_Stats.maxHp);
			healthPack->handled = true;
			EventBus::emit(HealthUpdated{ m_Stats.hp, m_Stats.maxHp });
		}
		break;
	}
	case PowerUpType::DoubleScore:
	{
		auto* doubleScore = reinterpret_cast<DoubleScoreData*>(powerUp);
		doubleScore->handled = true;
		EventBus::emit(DoubleScoreStarted{ doubleScore->getDuration() });
		break;
	}
	case PowerUpType::Bomb:
	{
		if (m_CanSpawnExplosion)
		{
			auto* bomb = reinterpret_cast<BombData*>(powerUp);
			auto p = Random::nextAnnulusPoint(20.0f, 10.0f);
			auto exp = m_ExplosionFactory.get();
			if (!exp) break;

			exp->setup(m_Transform->getPosition() + glm::vec3(p.x, 2.5f, p.y));
			bomb->handled = true;
		}
	}
	default:
	break;
	}
}

} // namespace game