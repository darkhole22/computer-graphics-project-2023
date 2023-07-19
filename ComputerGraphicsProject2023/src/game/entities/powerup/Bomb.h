#pragma once

#include "PickUp.h"

namespace game {

using namespace vulture;

class BombData : public PickUpData
{
public:
	virtual PickUpType getType() const override;
	virtual bool isHandled() const override;
	virtual void setHandled(bool handled) override;

	virtual ~BombData() = default;
private:
	bool m_Handled = false;
};


class Bomb : public PickUp<BombData>
{
public:
	static const String s_ModelName;
	static const String s_TextureName;
	static const String s_EmissionTextureName;
	static const String s_RoughnessTextureName;

	explicit Bomb(Ref<GameObject> gameObject);
};

} // namespace game