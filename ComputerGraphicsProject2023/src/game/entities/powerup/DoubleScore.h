#pragma once

#include "PowerUp.h"

namespace game {

using namespace vulture;

class DoubleScoreData : public PowerUpData
{
public:
	virtual PowerUpType getType() const override;
	virtual bool isHandled() const override;
	virtual void setHandled(bool handled) override;

	virtual f32 getDuration() const;

	virtual ~DoubleScoreData() = default;
private:
	f32 m_Duration = 20.0f;
	bool m_Handled = false;
};

class DoubleScore : public PowerUp<DoubleScoreData>
{
public:
	static const String s_ModelName;
	static const String s_TextureName;
	static const String s_EmissionTextureName;
	static const String s_RoughnessTextureName;

	explicit DoubleScore(Ref<GameObject> gameObject);
};

} // namespace game
