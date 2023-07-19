#pragma once

#include "vulture/core/Core.h"

#include "game/terrain/Terrain.h"
#include "game/entities/Factory.h"
#include "game/entities/CollisionMask.h"

#include <type_traits>

namespace game {

using namespace vulture;

enum class PickUpType
{
	None, HealthUp, DoubleScore, Bomb
};

class PickUpData
{
public:
	virtual PickUpType getType() const;
	virtual bool isHandled() const;
	virtual void setHandled(bool handled);

	virtual ~PickUpData();
};

template <typename T>
class PickUp
{
public:
	static_assert(std::is_base_of_v<PickUpData, T>);

	Ref<GameObject> gameObject;

	explicit PickUp(Ref<GameObject> gameObject)
		: gameObject(gameObject)
	{
		// This requires all power-ups to have the same size.
		// It's not a problem as we want them to be roughly equal in size.
		m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.0f, 2.0f));
		m_Hitbox->layerMask = POWER_UP_MASK;
		m_Hitbox->collisionMask = PLAYER_MASK;
		m_Hitbox->transform = gameObject->transform;
		m_Hitbox->data = &m_Data;
	}

	void setup(Ref<Terrain> terrain)
	{
		auto pos = gameObject->transform->getPosition();
		m_BaseHeight = terrain->getHeightAt(pos.x, pos.z) + c_BaseHeightOffset;

		Application::getScene()->addHitbox(m_Hitbox);
	}

	EntityStatus update(f32 dt)
	{
		m_DeltaHeight += dt;
		m_DeltaAngle += dt;

		auto pos = gameObject->transform->getPosition();
		gameObject->transform->setPosition(pos.x, m_BaseHeight + 0.35f * std::sin(m_DeltaHeight), pos.z);
		gameObject->transform->setRotation(0, m_DeltaAngle, 0);

		return m_Data.isHandled() ? EntityStatus::DEAD : EntityStatus::ALIVE;
	}

	~PickUp()
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
