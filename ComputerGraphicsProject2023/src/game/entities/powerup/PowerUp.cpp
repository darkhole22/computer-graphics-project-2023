#include "PowerUp.h"

namespace game {

PowerUpType PowerUpData::getType() const
{
	return PowerUpType::None;
}

PowerUpData::~PowerUpData() = default;

} // namespace game
