#pragma once

#include "game/GameState.h"
#include "vulture/event/Event.h"

namespace game {

struct GameStateChanged
{
	GameState gameState;
};

struct GodmodeToggled
{
	bool godmodeOn;
};

#define GAME_EVENTS 	STATIC_EVENT(GameStateChanged) \
						STATIC_EVENT(GodmodeToggled)
} // namespace game