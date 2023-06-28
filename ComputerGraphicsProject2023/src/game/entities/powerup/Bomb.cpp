#include "Bomb.h"
#include "game/entities/CollisionMask.h"

namespace game {

const String Bomb::s_ModelName = "bomb";
const String Bomb::s_TextureName = "bomb";
const String Bomb::s_EmissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME;
const String Bomb::s_RoughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME;

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