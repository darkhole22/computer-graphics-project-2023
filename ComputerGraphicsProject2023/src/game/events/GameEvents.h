#pragma once

#include "game/GameState.h"
#include "vulture/event/Event.h"
#include "vulture/util/Types.h"

namespace game {

using namespace vulture;

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

struct ExplosionStarted{};

struct ExplosionFinished{};

struct DoubleScoreOver{};

#define GAME_EVENTS		STATIC_EVENT(GameStateChanged) 		\
						STATIC_EVENT(GodmodeToggled)    	\
					 	STATIC_EVENT(ScoreUpdated)      	\
						STATIC_EVENT(DoubleScoreStarted)  	\
						STATIC_EVENT(DoubleScoreOver)   	\
						STATIC_EVENT(ExplosionStarted)		\
						STATIC_EVENT(ExplosionFinished)
} // namespace game