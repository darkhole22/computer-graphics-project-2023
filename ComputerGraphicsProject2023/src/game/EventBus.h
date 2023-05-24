#pragma once

#include "game/events/PlayerEvents.h"

#include "vulture/core/Core.h"

using namespace vulture;

namespace game {

class EventBus
{
	PLAYER_EVENTS

public:
	static void init();
	static void cleanup();

	inline static Ref<EventBus> getInstance() { return s_Instance; }

	~EventBus() = default;
private:
	static Ref<EventBus> s_Instance;

	EventBus() = default;
};

} // namespace game
