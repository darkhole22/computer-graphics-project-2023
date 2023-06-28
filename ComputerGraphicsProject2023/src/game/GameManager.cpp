#include "GameManager.h"

namespace game {

GameManager::GameManager(Ref<Terrain> terrain) :
		m_Terrain(terrain), m_Player(makeRef<Player>(terrain)),
		m_EnemyFactory(50, glm::rotate(glm::mat4(1), glm::half_pi<f32>(), glm::vec3(0, 1, 0))),
		m_PowerUpManager(m_Player, terrain),
		m_GameState(GameState::SETUP),
		m_DeathAudio("lose")
{
	m_Scene = Application::getScene();

	EventBus::addCallback([this](HealthUpdated event) {
		if (event.hp != 0) return;
		Application::getWindow()->setCursorMode(CursorMode::NORMAL);
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
			auto startingLocation = m_Player->transform->getPosition() + glm::vec3(p.x, 0.0f, p.y);

			enemy->setup(m_Player, startingLocation);
		}
	});
	m_WaveTimer->pause();
}

void GameManager::update(f32 dt)
{
	switch (m_GameState)
	{
	case GameState::SETUP:
	{
		m_Score = 0;
		EventBus::emit(ScoreUpdated{ m_Score });

		m_DoubleScoreActive = false;

		m_WaveTimer->play();
		m_PowerUpManager.start();

		setGameState(GameState::PLAYING);
		Application::getWindow()->setCursorMode(CursorMode::DISABLED);
		break;
	}
	case GameState::PLAYING:
	{
		m_EnemyFactory.update(dt);

		for (auto& enemy : m_EnemyFactory)
		{
			auto pos = enemy->m_GameObject->transform->getPosition();
			enemy->m_GameObject->transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + enemy->getFlyingHeight(), pos.z);
		}

		m_PowerUpManager.update(dt);
		m_Player->update(dt);

		if (Input::isActionJustPressed("TOGGLE_PAUSE"))
		{
			m_WaveTimer->pause();

			m_PowerUpManager.pause();

			setGameState(GameState::PAUSE);
			Application::getWindow()->setCursorMode(CursorMode::NORMAL);
		}

		break;
	}
	case GameState::PAUSE:
	if (Input::isActionJustPressed("TOGGLE_PAUSE"))
	{
		m_WaveTimer->play();

		m_PowerUpManager.start();

		setGameState(GameState::PLAYING);
		Application::getWindow()->setCursorMode(CursorMode::DISABLED);
	}
	break;
	case GameState::GAME_OVER:
	if (Input::isActionJustPressed("RESTART"))
	{
		m_DeathAudio.stop();
		beforeRestart();
		Application::getWindow()->setCursorMode(CursorMode::DISABLED);
	}
	break;
	}
}

void GameManager::setGameState(GameState gameState)
{
	if (gameState != m_GameState)
	{
		m_GameState = gameState;
		EventBus::emit(GameStateChanged{ m_GameState });
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
