#pragma once

#include "vulture/core/Core.h"
#include "game/events/GameEvents.h"
#include "game/events/PlayerEvents.h"
#include "game/events/EnemyEvents.h"

using namespace vulture;

namespace game {

/**
 * @brief A singleton Event Bus for all events related to game logic.
 */
class EventBus
{
	PLAYER_EVENTS
	ENEMY_EVENTS
	GAME_EVENTS

public:
	NO_COPY(EventBus)

	static void init();
	static void cleanup();

	~EventBus() = default;
private:
	static Ref<EventBus> s_Instance;

	EventBus() = default;
};

} // namespace game
