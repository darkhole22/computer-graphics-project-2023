#pragma once

#include "vulture/core/Core.h"
#include "game/entities/Factory.h"
#include "game/terrain/Terrain.h"
#include "game/entities/CollisionMask.h"

namespace game {

enum class PowerUpType
{
	None, HealthUp, DoubleScore, Bomb
};

class PowerUpData
{
public:
	virtual PowerUpType getType() const;

	bool handled = false;

	virtual ~PowerUpData();
};

template <typename T>
class PowerUp
{
public:
	Ref<GameObject> m_GameObject;

	explicit PowerUp(Ref<GameObject> gameObject)
		: m_GameObject(gameObject)
	{
		// This requires all power-ups to have the same size.
		// It's not a problem as we want them to be roughly equal in size.
		m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.0f, 2.0f));
		m_Hitbox->layerMask = POWER_UP_MASK;
		m_Hitbox->collisionMask = PLAYER_MASK;
		m_Hitbox->transform = m_GameObject->transform;
		m_Hitbox->data = &m_Data;
	}

	void setup(Ref<Terrain> terrain)
	{
		auto pos = m_GameObject->transform->getPosition();
		m_BaseHeight = terrain->getHeightAt(pos.x, pos.z) + c_BaseHeightOffset;

		Application::getScene()->addHitbox(m_Hitbox);
	}

	EntityStatus update(f32 dt)
	{
		m_DeltaHeight += dt;
		m_DeltaAngle += dt;

		auto pos = m_GameObject->transform->getPosition();
		m_GameObject->transform->setPosition(pos.x, m_BaseHeight + 0.35f * std::sin(m_DeltaHeight), pos.z);
		m_GameObject->transform->setRotation(0, m_DeltaAngle, 0);

		return m_Data.handled ? EntityStatus::DEAD : EntityStatus::ALIVE;
	}

	~PowerUp()
	{
		Application::getScene()->removeHitbox(m_Hitbox);
	}
private:
	Ref<HitBox> m_Hitbox;
	T m_Data;

	f32 m_DeltaHeight = 0.0f;
	f32 m_DeltaAngle = 0.0f;
	f32 m_BaseHeight;

	static constexpr f32 c_BaseHeightOffset = 1.5f;
};

} // namespace game
