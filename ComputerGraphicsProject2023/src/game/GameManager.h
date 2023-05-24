#pragma once

#include <random>

#include "game/entities/Enemy.h"
#include "game/entities/Player.h"
#include "game/entities/Factory.h"
#include "vulture/core/Core.h"
#include "game/ui/HUD.h"

namespace game {

class GameManager
{
public:
	GameManager()
	{
		m_Scene = Application::getScene();

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

			for(int i = 0; i <= 10; i++) {
				auto enemy = m_EnemyFactory->get();
				enemy->m_GameObject->tag = "ENEMY";
				auto startingLocation = m_Player->transform.getPosition() + glm::vec3(uni(rng), 0.0f, uni(rng));

				enemy->m_GameObject->transform.setPosition(startingLocation);
				enemy->m_GameObject->transform.setScale(3.0f, 3.0f, 3.0f);

				enemy->setup(m_Player);
			}
		});
	}

	void update(float dt)
	{
		m_EnemyFactory->update(dt);
		m_Player->update(dt);
	}

private:
	Scene* m_Scene = nullptr;

	Ref<Player> m_Player = nullptr;

	Ref<HUD> m_HUD = nullptr;

	Factory<Enemy>* m_EnemyFactory;

};

} // namespace game