#include "PickUp.h"

namespace game {

PickUpType PickUpData::getType() const
{
	return PickUpType::None;
}

bool PickUpData::isHandled() const
{
	return true;
}

void PickUpData::setHandled(bool handled)
{}

PickUpData::~PickUpData() = default;

} // namespace game
