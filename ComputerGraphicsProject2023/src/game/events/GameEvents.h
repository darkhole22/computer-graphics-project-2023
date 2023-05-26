#pragma once

#include "game/GameState.h"
#include "vulture/event/Event.h"

namespace game {

struct GameStateChanged
{
	GameState gameState;
};

#define GAME_EVENTS 	STATIC_EVENT(GameStateChanged)

} // namespace game