#include "GameManager.h"

namespace game {

GameManager::GameManager(Ref<Terrain> terrain)
{
	m_Scene = Application::getScene();
	m_Terrain = terrain;

	m_EnemyFactory = makeRef<Factory<Enemy>>(20);

	m_Player = makeRef<Player>();
	EventBus::addCallback([this](HealthUpdated event) { setGameState(event.hp == 0 ? GameState::GAME_OVER : GameState::PLAYING); });

	auto waveTween = m_Scene->makeTween();
	waveTween->loop();
	waveTween->addIntervalTweener(10.0f);
	waveTween->addCallbackTweener([this]() {
		std::random_device rd;     // Only used once to initialise (seed) engine
		std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(-30, 30); // Guaranteed unbiased

		for (int i = 0; i <= 10; i++)
		{
			auto enemy = m_EnemyFactory->get();
			enemy->m_GameObject->tag = "ENEMY";
			auto startingLocation = m_Player->transform.getPosition() + glm::vec3(uni(rng), 0.0f, uni(rng));

			enemy->m_GameObject->transform.setPosition(startingLocation);
			enemy->m_GameObject->transform.setScale(3.0f, 3.0f, 3.0f);

			enemy->setup(m_Player);
		}
	});
}

void GameManager::update(f32 dt)
{
	switch (m_GameState)
	{
	case GameState::PLAYING: {
		m_EnemyFactory->update(dt);

		for (auto &enemy: *m_EnemyFactory) {
			auto pos = enemy->m_GameObject->transform.getPosition();
			enemy->m_GameObject->transform.setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z), pos.z);
			enemy->m_Hitbox->transform = enemy->m_GameObject->transform;
			enemy->m_Hitbox->transform.setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + 1, pos.z);
		}

		m_Player->update(dt);

		auto pos = m_Player->transform.getPosition();
		m_Player->transform.setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z), pos.z);
		m_Player->m_Hitbox->transform = m_Player->transform;
		m_Player->m_Hitbox->transform.setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + 1, pos.z);

		if (Input::isActionJustPressed("TOGGLE_PAUSE")) {
			setGameState(GameState::PAUSE);
		}

		break;
	}
	case GameState::PAUSE:
		if (Input::isActionJustPressed("TOGGLE_PAUSE"))
		{
			setGameState(GameState::PLAYING);
		}
		break;
	case GameState::GAME_OVER:
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

} // namespace game
