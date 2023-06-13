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

struct DoubleScoreStarted
{
	f32 duration;
};

struct DoubleScoreOver{};

#define GAME_EVENTS		STATIC_EVENT(GameStateChanged) 	\
						STATIC_EVENT(GodmodeToggled)    \
					 	STATIC_EVENT(ScoreUpdated)      \
						STATIC_EVENT(DoubleScoreStarted)  \
						STATIC_EVENT(DoubleScoreOver)
} // namespace game