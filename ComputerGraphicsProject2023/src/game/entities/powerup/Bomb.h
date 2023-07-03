#pragma once

#include "PowerUp.h"

namespace game {

using namespace vulture;

class BombData : public PowerUpData
{
public:
	virtual PowerUpType getType() const override;
	virtual bool isHandled() const override;
	virtual void setHandled(bool handled) override;

	virtual ~BombData() = default;
private:
	f32 m_ExplosionRadius = 3.0f;
	bool m_Handled = false;
};


class Bomb : public PowerUp<BombData>
{
public:
	static const String s_ModelName;
	static const String s_TextureName;
	static const String s_EmissionTextureName;
	static const String s_RoughnessTextureName;

	explicit Bomb(Ref<GameObject> gameObject);
};

} // namespace game