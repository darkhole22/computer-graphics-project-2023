#pragma once

#include "vulture/core/Core.h"
#include "game/entities/Factory.h"
#include "game/terrain/Terrain.h"
#include "PowerUp.h"

namespace game {

using namespace vulture;

class HealthPackData : public PowerUpData
{
public:
	virtual PowerUpType getType() const;

	virtual u32 getHealth() const;

	virtual ~HealthPackData() = default;
private:
	u32 m_Health = 1;
};

class HealthPack
{
public:
	static const String s_ModelName;
	static const String s_TextureName;

	Ref<GameObject> m_GameObject;

	explicit HealthPack(Ref<GameObject> gameObject);

	void setup(Ref<Terrain> terrain);

	EntityStatus update(f32 dt);

	~HealthPack();
private:
	Ref<Terrain> m_Terrain = nullptr;
	Ref<HitBox> m_Hitbox;
	EntityStatus m_Status = EntityStatus::ALIVE;
	HealthPackData m_Data;

	f32 m_DeltaHeight = 0;
	f32 m_DeltaAngle = 0;
};

} // namespace game
