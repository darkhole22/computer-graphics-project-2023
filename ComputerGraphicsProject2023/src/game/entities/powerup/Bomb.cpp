#include "Bomb.h"
#include "game/entities/CollisionMask.h"

namespace game {

const String Bomb::s_ModelName = "bomb";
const String Bomb::s_TextureName = "bomb";

Bomb::Bomb(Ref<GameObject> gameObject) : PowerUp<BombData>(gameObject) {};

PowerUpType BombData::getType() const
{
	return PowerUpType::Bomb;
}

f32 BombData::getExplosionRadius() const
{
	return m_ExplosionRadius;
}

} // namespace game