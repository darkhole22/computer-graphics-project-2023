#pragma once

#include <random>

#include "Enemy.h"
#include "Player.h"
#include "vulture/core/Core.h"

namespace game {

class GameManager
{
public:
	GameManager()
	{
		m_Scene = Application::getScene();

		m_Player = makeRef<Player>();

		auto waveTween = m_Scene->makeTween();
		waveTween->loop();
		waveTween->addIntervalTweener(10.0f);
		waveTween->addCallbackTweener([this]() {
			std::random_device rd;     // Only used once to initialise (seed) engine
			std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
			std::uniform_int_distribution<int> uni(-30, 30); // Guaranteed unbiased

			for(int i = 0; i <= 10; i++) {
				auto randomLocation = m_Player->transform.getPosition() + glm::vec3(uni(rng), 0.0f, uni(rng));

				Ref<Enemy> enemy = makeRef<Enemy>();
				enemy->m_GameObject->transform.setPosition(randomLocation);
				enemy->m_GameObject->transform.setScale(3.0f, 3.0f, 3.0f);

				m_Enemies.push_back(enemy);
				m_Scene->addObject(enemy->m_GameObject);
			}
		});
	}

	void update(float dt)
	{
		m_Player->update(dt);

		for(auto e: m_Enemies) { e->update(dt); }
	}
private:
	Scene* m_Scene = nullptr;

	Ref<Player> m_Player = nullptr;
	std::vector<Ref<Enemy>> m_Enemies;
};

} // namespace game