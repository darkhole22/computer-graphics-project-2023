#include "HealthPack.h"

namespace game {

const String HealthPack::s_ModelName = "health";
const String HealthPack::s_TextureName = "health";

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
