#include "GameManager.h"

namespace game {

GameManager::GameManager(const TerrainGenerationConfig& terrainConfig) :
		m_Terrain(makeRef<Terrain>(terrainConfig)), m_Player(makeRef<Player>(m_Terrain)),
		m_EnemyFactory(15, glm::rotate(glm::mat4(1), glm::half_pi<f32>(), glm::vec3(0, 1, 0))),
		m_PowerUpManager(m_Player, m_Terrain),
		m_DeathAudio("lose")
{
	m_Scene = Application::getScene();

	EventBus::addCallback([this](HealthUpdated event) {
		if (event.hp != 0) return;
		onGameOver();
	});

	EventBus::addCallback([this](EnemyDied e) { onEnemyDied(e); });
	EventBus::addCallback([this](DoubleScoreStarted e) { onDoubleScoreStarted(e); });

	m_WaveTimer = m_Scene->makeTimer(20, false);
	m_WaveTimer->addCallback([this](const TimerTimeoutEvent&) {
		for (int i = 0; i < 10; i++)
		{
			auto p = Random::nextAnnulusPoint(100.f);
			auto enemy = m_EnemyFactory.get();
			if (!enemy) break;

			auto startingLocation = m_Player->transform->getPosition() + glm::vec3(p.x, 0.0f, p.y);
			enemy->setup(m_Player, m_Terrain, startingLocation);
		}
	});
	m_WaveTimer->pause();

	setGameState(GameState::TITLE);
}

void GameManager::update(f32 dt)
{
	switch (m_GameState)
	{
	case GameState::TITLE:
	if (Input::isActionJustPressed("FIRE"))
	{
		setGameState(GameState::SETUP);
	}
	break;
	case GameState::SETUP:
	{
		m_Score = 0;
		EventBus::emit(ScoreUpdated{ m_Score });

		m_DoubleScoreActive = false;

		m_WaveTimer->play();
		m_PowerUpManager.start();

		setGameState(GameState::PLAYING);
		break;
	}
	case GameState::PLAYING:
	{
		m_EnemyFactory.update(dt);
		m_PowerUpManager.update(dt);
		m_Player->update(dt);

		if (Input::isActionJustPressed("TOGGLE_PAUSE"))
		{
			m_WaveTimer->pause();
			m_PowerUpManager.pause();

			setGameState(GameState::PAUSE);
		}

		break;
	}
	case GameState::PAUSE:
	if (Input::isActionJustPressed("TOGGLE_PAUSE"))
	{
		m_WaveTimer->play();
		m_PowerUpManager.start();

		setGameState(GameState::PLAYING);
	}
	break;
	case GameState::GAME_OVER:
	if (Input::isActionJustPressed("RESTART"))
	{
		m_DeathAudio.stop();
		beforeRestart();
	}
	break;
	}

	auto cameraPos = m_Scene->getCamera()->position;
	m_Terrain->setReferencePosition({ cameraPos.x, cameraPos.z });

	m_Terrain->update(dt);
}

void GameManager::setGameState(GameState gameState)
{
	if (gameState != m_GameState)
	{
		m_GameState = gameState;
		EventBus::emit(GameStateChanged{ m_GameState });
	}

	if (m_GameState == GameState::PLAYING)
	{
		Application::getWindow()->setCursorMode(CursorMode::DISABLED);
	}
	else
	{
		Application::getWindow()->setCursorMode(CursorMode::NORMAL);
	}
}

void GameManager::onGameOver()
{
	m_WaveTimer->reset(false);
	Application::getWindow()->setCursorMode(CursorMode::NORMAL);

	m_PowerUpManager.pause();

	if (m_GameState != GameState::GAME_OVER)
		m_DeathAudio.play();
	setGameState(GameState::GAME_OVER);
}

void GameManager::beforeRestart()
{
	m_EnemyFactory.reset();
	m_PowerUpManager.reset();
	m_Player->reset();

	setGameState(GameState::SETUP);
}

void GameManager::onEnemyDied(EnemyDied event)
{
	m_Score += m_DoubleScoreActive ? 200 : 100;
	EventBus::emit(ScoreUpdated{ m_Score });
}

void GameManager::onDoubleScoreStarted(DoubleScoreStarted e)
{
	m_DoubleScoreActive = true;
	m_Scene->makeTimer(e.duration)->addCallback([this](const TimerTimeoutEvent&) {
		m_DoubleScoreActive = false;
		EventBus::emit(DoubleScoreOver{});
	});
}

} // namespace game
