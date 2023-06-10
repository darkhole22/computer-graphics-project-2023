#pragma once

#include "vulture/event/Event.h"
#include "vulture/util/Types.h"

using namespace vulture;

namespace game {

struct HealthUpdated
{
	u32 hp;
	u32 maxHp;
};

struct BulletShot {};

#define PLAYER_EVENTS 	STATIC_EVENT(HealthUpdated) \
						STATIC_EVENT(BulletShot)

} // namespace game