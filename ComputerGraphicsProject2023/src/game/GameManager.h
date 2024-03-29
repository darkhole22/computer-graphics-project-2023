#pragma once

#include "vulture/core/Core.h"
#include "vulture/audio/AudioPlayer.h"

#include "game/GameState.h"
#include "game/entities/Player.h"
#include "game/entities/Enemy.h"
#include "game/entities/Factory.h"
#include "game/entities/powerup/HealthPack.h"
#include "game/entities/powerup/DoubleScore.h"
#include "game/entities/PickUpManager.h"
#include "game/ui/HUD.h"
#include "game/terrain/Terrain.h"

namespace game {

using namespace vulture;

class GameManager
{
public:
	explicit GameManager(const TerrainGenerationConfig& terrainConfig);

	void update(f32 dt);

	~GameManager() = default;
private:
	Scene* m_Scene = nullptr;
	Ref<Terrain> m_Terrain = nullptr;

	Ref<Player> m_Player = nullptr;
	u32 m_EnemyWaveSize = 10;
	Factory<Enemy> m_EnemyFactory;

	PickUpManager m_PickUpManager;
	Ref<Timer> m_WaveTimer;

	GameState m_GameState;
	u32 m_Score;

	AudioPlayer m_DeathAudio;

	void setGameState(GameState gameState);

	void onGameOver();

	void onEnemyDied(EnemyDied event);
	void onDoubleScoreStarted(DoubleScoreStarted event);
};

} // namespace game