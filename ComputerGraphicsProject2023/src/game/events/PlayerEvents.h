#pragma once

#include "vulture/event/Event.h"

namespace game {

struct HealthUpdated
{
	int hp;
	int maxHp;
};

struct AmmoUpdated
{
	int ammo;
	int maxAmmo;
};

#define PLAYER_EVENTS 	STATIC_EVENT(HealthUpdated) \
						STATIC_EVENT(AmmoUpdated)

} // namespace game