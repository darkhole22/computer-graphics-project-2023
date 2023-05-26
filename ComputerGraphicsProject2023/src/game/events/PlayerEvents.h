#pragma once

#include "vulture/event/Event.h"
#include "vulture/util/Types.h"

namespace game {

using namespace vulture;

struct HealthUpdated
{
	u32 hp;
	u32 maxHp;
};

struct AmmoUpdated
{
	u32 ammo;
	u32 maxAmmo;
};

#define PLAYER_EVENTS 	STATIC_EVENT(HealthUpdated) \
						STATIC_EVENT(AmmoUpdated)

} // namespace game