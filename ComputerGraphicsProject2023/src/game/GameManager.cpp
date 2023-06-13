#include "GameManager.h"

#include "vulture/core/Logger.h"

#include "vulture/util/ScopeTimer.h"

namespace game {

GameManager::GameManager(Ref<Terrain> terrain) :
	m_Terrain(terrain), m_EnemyFactory(20), m_HealthPackFactory(50),
	m_GameState(GameState::SETUP)
{
	m_Scene = Application::getScene();

	m_Player = makeRef<Player>();
	EventBus::addCallback([this](HealthUpdated event) {
		if (event.hp != 0) return;
		Application::getWindow()->setCursorMode(CursorMode::NORMAL);
		onGameOver();
	});

	EventBus::addCallback([this](EnemyDied e) { onEnemyDied(e); });

	m_WaveTween = m_Scene->makeTween();
	m_WaveTween->loop();
	m_WaveTween->addIntervalTweener(10.0f);
	m_WaveTween->addCallbackTweener([this]() {
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_real_distribution<f32> uni{};

		for (int i = 0; i <= 10; i++)
		{
			f32 theta = uni(rng) * glm::two_pi<f32>();
			f32 r = std::sqrt(0.9f * uni(rng) + 0.1f);
			auto enemy = m_EnemyFactory.get();
			auto startingLocation = m_Player->transform->getPosition() + glm::vec3(
				r * 150.0f * std::cos(theta),
				0.0f,
				r * 150.0f * std::sin(theta)
			);

			enemy->m_GameObject->transform->setPosition(startingLocation);
			enemy->setup(m_Player, startingLocation);
		}
	});
	m_WaveTween->pause();

	m_HealthPackTween = m_Scene->makeTimer(20, false);
	m_HealthPackTween->addCallback([this](const TimerTimeoutEvent&) {
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_real_distribution<f32> uni{};

		for (int i = 0; i < 2; i++)
		{
			f32 theta = uni(rng) * glm::two_pi<f32>();
			f32 r = std::sqrt(0.9f * uni(rng) + 0.1f);
			auto pack = m_HealthPackFactory.get();
			auto startingLocation = m_Player->transform->getPosition() + glm::vec3(
				r * 100.0f * std::cos(theta),
				0.0f,
				r * 100.0f * std::sin(theta)
			);

			pack->m_GameObject->transform->setPosition(startingLocation);
			pack->setup(m_Terrain);
		}
	});
	m_HealthPackTween->pause();
}

void GameManager::update(f32 dt)
{
	switch (m_GameState)
	{
	case GameState::SETUP:
	m_Score = 0;
	EventBus::emit(ScoreUpdated{ m_Score });

	m_WaveTween->play();
	m_HealthPackTween->play();
	setGameState(GameState::PLAYING);
	break;
	case GameState::PLAYING:
	{
		m_EnemyFactory.update(dt);

		for (auto& enemy : m_EnemyFactory)
		{
			auto pos = enemy->m_GameObject->transform->getPosition();
			enemy->m_GameObject->transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + Enemy::s_FlyingHeight, pos.z);
		}

		m_HealthPackFactory.update(dt);

		m_Player->update(dt);

		auto pos = m_Player->transform->getPosition();
		m_Player->transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z), pos.z);

		if (Input::isActionJustPressed("TOGGLE_PAUSE"))
		{
			m_WaveTween->pause();
			m_HealthPackTween->pause();
			setGameState(GameState::PAUSE);
			Application::getWindow()->setCursorMode(CursorMode::NORMAL);
		}

		break;
	}
	case GameState::PAUSE:
	if (Input::isActionJustPressed("TOGGLE_PAUSE"))
	{
		m_WaveTween->play();
		m_HealthPackTween->play();
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
	m_WaveTween->reset(false);
	Application::getWindow()->setCursorMode(CursorMode::NORMAL);
	m_HealthPackTween->reset();
	setGameState(GameState::GAME_OVER);
}

void GameManager::beforeRestart()
{
	m_EnemyFactory.reset();
	m_HealthPackFactory.reset();
	m_Player->reset();

	setGameState(GameState::SETUP);
}

void GameManager::onEnemyDied(EnemyDied event)
{
	m_Score += 100;
	EventBus::emit(ScoreUpdated{ m_Score });
}

} // namespace game
