#include "GameManager.h"

namespace game {

GameManager::GameManager(Ref<Terrain> terrain) {
	m_Scene = Application::getScene();
	m_Terrain = terrain;

	m_EnemyFactory = new Factory<Enemy>(20);

	m_Player = makeRef<Player>();
	m_HUD = makeRef<HUD>(m_Player);

	auto waveTween = m_Scene->makeTween();
	waveTween->loop();
	waveTween->addIntervalTweener(10.0f);
	waveTween->addCallbackTweener([this]() {
		std::random_device rd;     // Only used once to initialise (seed) engine
		std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(-30, 30); // Guaranteed unbiased

		for (int i = 0; i <= 10; i++) {
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
	m_EnemyFactory->update(dt);

	for (auto e: *m_EnemyFactory)
	{
		auto pos = e->m_GameObject->transform.getPosition();
		e->m_GameObject->transform.setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z), pos.z);
	}

	m_Player->update(dt);

	auto pos = m_Player->transform.getPosition();
	m_Player->transform.setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z), pos.z);
}

} // namespace game
