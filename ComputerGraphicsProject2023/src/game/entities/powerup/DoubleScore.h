#pragma once

#include "PowerUp.h"

namespace game {

using namespace vulture;

class DoubleScoreData : public PowerUpData
{
public:
	virtual PowerUpType getType() const;

	virtual f32 getDuration() const;

	virtual ~DoubleScoreData() = default;
private:
	f32 m_Duration = 20.0f;
};

class DoubleScore : public PowerUp<DoubleScoreData>
{
public:
	static const String s_ModelName;
	static const String s_TextureName;

	explicit DoubleScore(Ref<GameObject> gameObject);
};

} // namespace game
