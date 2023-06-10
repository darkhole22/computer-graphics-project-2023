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

struct DashesUpdated
{
	u32 dashes;
	u32 maxDashes;
};

#define PLAYER_EVENTS 	STATIC_EVENT(HealthUpdated) 	\
						STATIC_EVENT(BulletShot)        \
						STATIC_EVENT(DashesUpdated)

} // namespace game