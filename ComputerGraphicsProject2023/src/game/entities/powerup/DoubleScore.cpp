#include "DoubleScore.h"

namespace game {

const String DoubleScore::s_ModelName = "star";
const String DoubleScore::s_TextureName = "star";

DoubleScore::DoubleScore(Ref<GameObject> gameObject) : PowerUp<DoubleScoreData>(gameObject) {};

PowerUpType DoubleScoreData::getType() const
{
	return PowerUpType::DoubleScore;
}

f32 DoubleScoreData::getDuration() const
{
	return m_Duration;
}

} // namespace game
