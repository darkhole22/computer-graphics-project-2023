#pragma once

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

class HealthPack : public PowerUp<HealthPackData>
{
public:
	static const String s_ModelName;
	static const String s_TextureName;

	explicit HealthPack(Ref<GameObject> gameObject);
};

} // namespace game
