#include "PowerUp.h"

namespace game {

PowerUpType PowerUpData::getType() const
{
	return PowerUpType::None;
}

bool PowerUpData::isHandled() const
{
	return true;
}

void PowerUpData::setHandled(bool handled)
{}

PowerUpData::~PowerUpData() = default;

} // namespace game
