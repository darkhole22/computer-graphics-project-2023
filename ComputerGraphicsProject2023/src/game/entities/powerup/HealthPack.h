#pragma once

#include "PowerUp.h"

namespace game {

using namespace vulture;

class HealthPackData : public PowerUpData
{
public:
	virtual PowerUpType getType() const override;
	virtual bool isHandled() const override;
	virtual void setHandled(bool handled) override;

	virtual u32 getHealth() const;

	virtual ~HealthPackData() = default;
private:
	u32 m_Health = 1;
	bool m_Handled = false;
};

class HealthPack : public PowerUp<HealthPackData>
{
public:
	static const String s_ModelName;
	static const String s_TextureName;
	static const String s_EmissionTextureName;
	static const String s_RoughnessTextureName;

	explicit HealthPack(Ref<GameObject> gameObject);
};

} // namespace game
