#pragma once

#include <utility>

#include "game/entities/Player.h"
#include "game/entities/Factory.h"
#include "vulture/core/Core.h"
#include "vulture/util/Random.h"

namespace game {

class PowerUpHandler
{
public:
	template<typename T>
	static PowerUpHandler create(Ref<Player> player, Ref<Terrain> terrain, f32 cooldown, u32 amount)
	{
		auto factory = makeRef<Factory<T>>(10);

		auto updateImpl = [factory] (f32 dt) { factory->update(dt); };
		auto resetImpl = [factory] () { factory->reset(); };

		auto timer = Application::getScene()->makeTimer(cooldown, false);
		timer->addCallback([=](const TimerTimeoutEvent&) {
			for (u32 i = 0; i < amount; i++)
			{
				auto p = Random::nextAnnulusPoint(100.f);
				auto powerUp = factory->get();
				auto startingLocation = player->transform->getPosition() + glm::vec3(p.x, 0.0f, p.y);

				powerUp->m_GameObject->transform->setPosition(startingLocation);
				powerUp->setup(terrain);
			}
		});
		timer->pause();

		return PowerUpHandler(updateImpl, resetImpl, timer);
	}

	void pause() { m_Timer->pause(); }

	void play() { m_Timer->play(); }

	void update(f32 dt) { m_UpdateImpl(dt); }

	void reset()
	{
		m_Timer->reset();
		m_ResetImpl();
	}

	~PowerUpHandler()
	{
		m_ResetImpl();
	}
private:
	Ref<Timer> m_Timer;

	std::function<void (f32)> m_UpdateImpl;
	std::function<void ()> m_ResetImpl;

	PowerUpHandler(std::function<void (f32)> updateImpl, std::function<void ()> resetImpl, Ref<Timer> timer) :
		 m_Timer(std::move(timer)), m_UpdateImpl(std::move(updateImpl)),m_ResetImpl(std::move(resetImpl))
	{
	}
};

class PowerUpManager
{
public:
	PowerUpManager(Ref<Player> player, Ref<Terrain> terrain);

	void start();
	void pause();

	void update(f32 dt);

	void reset();

	~PowerUpManager() = default;
private:
	Ref<Player> m_Player;
	Ref<Terrain> m_Terrain;

	std::vector<PowerUpHandler> m_Handlers;
};

} // namespace game
