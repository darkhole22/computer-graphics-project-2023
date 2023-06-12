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

struct BulletShot {};

struct DashesUpdated
{
	u32 dashes;
	u32 maxDashes;
};

struct LevelUp
{
	String message;
};

#define PLAYER_EVENTS 	STATIC_EVENT(HealthUpdated) 	\
						STATIC_EVENT(BulletShot)        \
						STATIC_EVENT(DashesUpdated)		\
						STATIC_EVENT(LevelUp)

} // namespace game