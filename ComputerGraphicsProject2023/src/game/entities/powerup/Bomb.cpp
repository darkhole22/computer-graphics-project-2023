#include "Bomb.h"
#include "game/entities/CollisionMask.h"

namespace game {

const String Bomb::s_ModelName = "bomb";
const String Bomb::s_TextureName = "bomb";
const String Bomb::s_EmissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME;
const String Bomb::s_RoughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME;

Bomb::Bomb(Ref<GameObject> gameObject) : PickUp<BombData>(gameObject) {};

PickUpType BombData::getType() const
{
	return PickUpType::Bomb;
}

bool BombData::isHandled() const
{
	return m_Handled;
}

void BombData::setHandled(bool handled)
{
	m_Handled = handled;
}

} // namespace game