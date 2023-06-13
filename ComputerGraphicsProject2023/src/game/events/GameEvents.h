#pragma once

#include "game/GameState.h"
#include "vulture/event/Event.h"
#include "vulture/util/Types.h"

using namespace vulture;

namespace game {

struct GameStateChanged
{
	GameState gameState;
};

struct GodmodeToggled
{
	bool godmodeOn;
};

struct ScoreUpdated
{
	u32 score;
};

struct DoubleExpStarted
{
	f32 duration;
};

struct DoubleExpOver{};

#define GAME_EVENTS		STATIC_EVENT(GameStateChanged) 	\
						STATIC_EVENT(GodmodeToggled)    \
					 	STATIC_EVENT(ScoreUpdated)      \
						STATIC_EVENT(DoubleExpStarted)  \
						STATIC_EVENT(DoubleExpOver)
} // namespace game