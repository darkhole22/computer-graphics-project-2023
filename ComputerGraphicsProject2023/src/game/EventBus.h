#pragma once

#include "game/events/PlayerEvents.h"

#include "vulture/core/Core.h"

using namespace vulture;

namespace game {

class EventBus
{
PLAYER_EVENTS

public:
	inline static Ref<EventBus> getInstance()
	{
		if (s_Instance.expired())
		{
			s_Instance = Ref<EventBus>(new EventBus());
		}

		return s_Instance.lock();
	}

	~EventBus() = default;

private:
	static WRef<EventBus> s_Instance;

	EventBus() = default;
};

} // namespace game
