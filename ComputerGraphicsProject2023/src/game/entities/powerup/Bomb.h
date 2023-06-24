#pragma once

#include "vulture/core/Core.h"
#include "PowerUp.h"
#include "game/terrain/Terrain.h"
#include "game/entities/Factory.h"

namespace game {

using namespace vulture;

class BombData : public PowerUpData
{
public:
	virtual PowerUpType getType() const;

	virtual f32 getExplosionRadius() const;

	virtual ~BombData() = default;
private:
	f32 m_ExplosionRadius = 3.0f;
};


class Bomb
{
public:
	static const String s_ModelName;
	static const String s_TextureName;

	Ref<GameObject> m_GameObject;

	explicit Bomb(Ref<GameObject> gameObject);

	void setup(Ref<Terrain> terrain);

	EntityStatus update(f32 dt);

	~Bomb();
private:
	Ref<Terrain> m_Terrain = nullptr;
	Ref<HitBox> m_Hitbox;
	EntityStatus m_Status = EntityStatus::ALIVE;
	BombData m_Data;

	f32 m_DeltaHeight = 0;
	f32 m_DeltaAngle = 0;
};

} // namespace game