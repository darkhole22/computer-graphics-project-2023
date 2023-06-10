#include "GameManager.h"

namespace game {

GameManager::GameManager(Ref<Terrain> terrain)
{
	m_Scene = Application::getScene();
	m_Terrain = terrain;

	m_EnemyFactory = makeRef<Factory<Enemy>>(20);

	m_Player = makeRef<Player>();
	EventBus::addCallback([this](HealthUpdated event) {
		if (event.hp != 0) return;
		Application::getWindow()->setCursorMode(CursorMode::NORMAL);
		onGameOver();
	});

	m_WaveTween = m_Scene->makeTween();
	m_WaveTween->loop();
	m_WaveTween->addIntervalTweener(10.0f);
	m_WaveTween->addCallbackTweener([this]() {
		std::random_device rd;     // Only used once to initialise (seed) engine
		std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(-150, 150); // Guaranteed unbiased

		for (int i = 0; i <= 10; i++)
		{
			auto enemy = m_EnemyFactory->get();
			auto startingLocation =
					m_Player->transform->getPosition() +
					glm::vec3(uni(rng), 0.0f, uni(rng));

			enemy->m_GameObject->transform->setPosition(startingLocation);
			enemy->setup(m_Player, startingLocation);
		}
	});
	m_WaveTween->reset(false);

	m_GameState = GameState::SETUP;
}

void GameManager::update(f32 dt)
{
	switch (m_GameState)
	{
	case GameState::SETUP:
		m_WaveTween->play();
		setGameState(GameState::PLAYING);
		break;
	case GameState::PLAYING:
	{
		m_EnemyFactory->update(dt);

		for (auto& enemy : *m_EnemyFactory)
		{
			auto pos = enemy->m_GameObject->transform->getPosition();
			enemy->m_GameObject->transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + Enemy::s_FlyingHeight, pos.z);
		}

		m_Player->update(dt);

		auto pos = m_Player->transform->getPosition();
		m_Player->transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z), pos.z);

		if (Input::isActionJustPressed("TOGGLE_PAUSE"))
		{
			setGameState(GameState::PAUSE);
			Application::getWindow()->setCursorMode(CursorMode::NORMAL);
		}

		break;
	}
	case GameState::PAUSE:
		if (Input::isActionJustPressed("TOGGLE_PAUSE"))
		{
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
	setGameState(GameState::GAME_OVER);
}

void GameManager::beforeRestart()
{
	m_EnemyFactory->reset();
	m_Player->reset();

	setGameState(GameState::SETUP);
}

} // namespace game
