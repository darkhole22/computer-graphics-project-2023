#pragma once

#include "vulture/core/Core.h"
#include "vulture/util/Random.h"

#include "game/entities/Player.h"
#include "game/entities/Factory.h"

#include <utility>

namespace game {

using namespace vulture;

class PickUpHandler
{
public:
	template<typename T>
	static PickUpHandler create(Ref<Player> player, Ref<Terrain> terrain, f32 cooldown, u32 amount);

	void update(f32 dt);

	void play();
	void pause();
	void reset();

	~PickUpHandler();
private:
	Ref<Timer> m_Timer;

	std::function<void(f32)> m_UpdateImpl;
	std::function<void()> m_ResetImpl;

	PickUpHandler(std::function<void(f32)> updateImpl, std::function<void()> resetImpl, Ref<Timer> timer) :
		m_Timer(std::move(timer)), m_UpdateImpl(std::move(updateImpl)), m_ResetImpl(std::move(resetImpl))
	{}
};

class PickUpManager
{
public:
	PickUpManager(Ref<Player> player, Ref<Terrain> terrain);

	void start();
	void pause();

	void update(f32 dt);

	void reset();

	~PickUpManager() = default;
private:
	Ref<Player> m_Player;
	Ref<Terrain> m_Terrain;

	std::vector<PickUpHandler> m_Handlers;
};

template<typename T>
inline PickUpHandler PickUpHandler::create(Ref<Player> player, Ref<Terrain> terrain, f32 cooldown, u32 amount)
{
	auto factory = makeRef<Factory<T>>(5);

	auto updateImpl = [factory](f32 dt) { factory->update(dt); };
	auto resetImpl = [factory]() { factory->reset(); };

	auto timer = Application::getScene()->makeTimer(cooldown, false);
	timer->addCallback([=](const TimerTimeoutEvent&) {
		for (u32 i = 0; i < amount; i++)
		{
			auto p = Random::nextAnnulusPoint(100.f);
			auto pickUp = factory->get();
			if (!pickUp) break;

			auto startingLocation = player->getPosition() + glm::vec3(p.x, 0.0f, p.y);

			pickUp->gameObject->transform->setPosition(startingLocation);
			pickUp->setup(terrain);
		}
	});
	timer->pause();

	return PickUpHandler(updateImpl, resetImpl, timer);
}

} // namespace game
