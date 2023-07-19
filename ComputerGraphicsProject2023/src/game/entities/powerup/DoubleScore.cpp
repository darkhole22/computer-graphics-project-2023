#include "DoubleScore.h"

namespace game {

const String DoubleScore::s_ModelName = "star";
const String DoubleScore::s_TextureName = "star";
const String DoubleScore::s_EmissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME;
const String DoubleScore::s_RoughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME;

DoubleScore::DoubleScore(Ref<GameObject> gameObject) : PickUp<DoubleScoreData>(gameObject) {};

PickUpType DoubleScoreData::getType() const
{
	return PickUpType::DoubleScore;
}

bool DoubleScoreData::isHandled() const
{
	return m_Handled;
}

void DoubleScoreData::setHandled(bool handled)
{
	m_Handled = handled;
}

f32 DoubleScoreData::getDuration() const
{
	return m_Duration;
}

} // namespace game
