#pragma once

#include "game/entities/Enemy.h"
#include "game/entities/Player.h"
#include "game/entities/Factory.h"
#include "vulture/core/Core.h"
#include "game/ui/HUD.h"
#include "game/terrain/Terrain.h"
#include "game/GameState.h"
#include "game/entities/powerup/HealthPack.h"
#include "vulture/util/ScopeTimer.h"
#include "game/Random.h"
#include "game/entities/powerup/DoubleExp.h"

namespace game {

class GameManager
{
public:
	explicit GameManager(Ref<Terrain> terrain);

	void update(f32 dt);
private:
	Scene* m_Scene = nullptr;
	Ref<Terrain> m_Terrain = nullptr;

	Ref<Player> m_Player = nullptr;
	Factory<Enemy> m_EnemyFactory;
	Factory<HealthPack> m_HealthPackFactory;
	Factory<DoubleExp> m_DoubleExpFactory;

	Ref<Timer> m_WaveTimer;
	Ref<Timer> m_HealthPackTimer;
	Ref<Timer> m_DoubleExpTimer;

	GameState m_GameState;
	u32 m_Score;
	bool m_DoubleExpActive;

	void setGameState(GameState gameState);

	void onGameOver();

	void beforeRestart();

	void onEnemyDied(EnemyDied event);
	void onDoubleExpStarted(DoubleExpStarted e);


};

} // namespace game