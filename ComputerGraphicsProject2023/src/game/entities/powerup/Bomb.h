#pragma once

#include "PowerUp.h"


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


class Bomb : public PowerUp<BombData>
{
public:
	static const String s_ModelName;
	static const String s_TextureName;

	explicit Bomb(Ref<GameObject> gameObject);
};

} // namespace game