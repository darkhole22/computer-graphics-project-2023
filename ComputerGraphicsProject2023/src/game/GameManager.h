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
#include "vulture/util/Random.h"
#include "vulture/audio/AudioPlayer.h"
#include "game/entities/powerup/DoubleScore.h"
#include "game/entities/PowerUpManager.h"

namespace game {

class GameManager
{
public:
	explicit GameManager(Ref<Terrain> terrain);

	void update(f32 dt);

	~GameManager() = default;
private:
	Scene* m_Scene = nullptr;
	Ref<Terrain> m_Terrain = nullptr;

	Ref<Player> m_Player = nullptr;

	Factory<Enemy> m_EnemyFactory;
	PowerUpManager m_PowerUpManager;

	Ref<Timer> m_WaveTimer;

	GameState m_GameState;
	u32 m_Score;
	bool m_DoubleScoreActive;

	AudioPlayer m_DeathAudio;

	void setGameState(GameState gameState);

	void onGameOver();

	void beforeRestart();

	void onEnemyDied(EnemyDied event);
	void onDoubleScoreStarted(DoubleScoreStarted e);
};

} // namespace game