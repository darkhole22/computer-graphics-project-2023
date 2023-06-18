#include "GameManager.h"

namespace game {

GameManager::GameManager(Ref<Terrain> terrain) :
	m_Terrain(terrain), m_EnemyFactory(50),
	m_HealthPackFactory(10), m_DoubleScoreFactory(10),
	m_GameState(GameState::SETUP)
{
	m_Scene = Application::getScene();

	m_Player = makeRef<Player>(m_Terrain);
	EventBus::addCallback([this](HealthUpdated event) {
		if (event.hp != 0) return;
		Application::getWindow()->setCursorMode(CursorMode::NORMAL);
		onGameOver();
	});

	EventBus::addCallback([this](EnemyDied e) { onEnemyDied(e); });
	EventBus::addCallback([this](DoubleScoreStarted e) { onDoubleScoreStarted(e); });

	m_WaveTimer = m_Scene->makeTimer(20, false);
	m_WaveTimer->addCallback([this](const TimerTimeoutEvent&) {
		for (int i = 0; i <= 10; i++)
		{
			auto p = Random::nextAnnulusPoint(100.f);
			auto enemy = m_EnemyFactory.get();
			auto startingLocation = m_Player->transform->getPosition() + glm::vec3(p.x, 0.0f, p.y);

			enemy->m_GameObject->transform->setPosition(startingLocation);
			enemy->setup(m_Player, startingLocation);
		}
	});
	m_WaveTimer->pause();

	m_HealthPackTimer = m_Scene->makeTimer(40, false);
	m_HealthPackTimer->addCallback([this](const TimerTimeoutEvent&) {
		for (int i = 0; i < 2; i++)
		{
			auto p = Random::nextAnnulusPoint(100.f);
			auto pack = m_HealthPackFactory.get();
			auto startingLocation = m_Player->transform->getPosition() + glm::vec3(p.x, 0.0f, p.y);

			pack->m_GameObject->transform->setPosition(startingLocation);
			pack->setup(m_Terrain);
		}
	});
	m_HealthPackTimer->pause();

	m_DoubleScoreTimer = m_Scene->makeTimer(30, false);
	m_DoubleScoreTimer->addCallback([this](const TimerTimeoutEvent&) {
		for (int i = 0; i < 1; i++)
		{
			auto p = Random::nextAnnulusPoint(100.f);
			auto doubleScore = m_DoubleScoreFactory.get();
			auto startingLocation = m_Player->transform->getPosition() + glm::vec3(p.x, 0.0f, p.y);

			doubleScore->m_GameObject->transform->setPosition(startingLocation);
			doubleScore->setup(m_Terrain);
		}
	});
	m_DoubleScoreTimer->pause();
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
		m_HealthPackTimer->play();
		m_DoubleScoreTimer->play();
		setGameState(GameState::PLAYING);
		break;
	}
	case GameState::PLAYING:
	{
		m_EnemyFactory.update(dt);

		for (auto& enemy : m_EnemyFactory)
		{
			auto pos = enemy->m_GameObject->transform->getPosition();
			enemy->m_GameObject->transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + Enemy::s_FlyingHeight, pos.z);
		}

		m_HealthPackFactory.update(dt);
		m_DoubleScoreFactory.update(dt);

		m_Player->update(dt);



		if (Input::isActionJustPressed("TOGGLE_PAUSE"))
		{
			m_WaveTimer->pause();
			m_HealthPackTimer->pause();
			m_DoubleScoreTimer->pause();

			setGameState(GameState::PAUSE);
			Application::getWindow()->setCursorMode(CursorMode::NORMAL);
		}

		break;
	}
	case GameState::PAUSE:
	if (Input::isActionJustPressed("TOGGLE_PAUSE"))
	{
		m_WaveTimer->play();
		m_HealthPackTimer->play();
		m_DoubleScoreTimer->play();

		setGameState(GameState::PLAYING);
		Application::getWindow()->setCursorMode(CursorMode::DISABLED);
	}
	break;
	case GameState::GAME_OVER:
	if (Input::isActionJustPressed("RESTART"))
	{
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
	m_HealthPackTimer->reset();
	setGameState(GameState::GAME_OVER);
}

void GameManager::beforeRestart()
{
	m_EnemyFactory.reset();
	m_HealthPackFactory.reset();
	m_DoubleScoreFactory.reset();

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
