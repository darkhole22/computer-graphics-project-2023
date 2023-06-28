#include "HealthPack.h"

namespace game {

const String HealthPack::s_ModelName = "health";
const String HealthPack::s_TextureName = "health";
const String HealthPack::s_EmissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME;
const String HealthPack::s_RoughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME;

HealthPack::HealthPack(Ref<GameObject> gameObject) : PowerUp<HealthPackData>(gameObject) {};

PowerUpType HealthPackData::getType() const
{
	return PowerUpType::HealthUp;
}

u32 HealthPackData::getHealth() const
{
	return m_Health;
}

} // namespace game
