#include "PickUpManager.h"

#include "game/entities/powerup/Bomb.h"
#include "game/entities/powerup/HealthPack.h"
#include "game/entities/powerup/DoubleScore.h"

namespace game {

PickUpManager::PickUpManager(Ref<Player> player, Ref<Terrain> terrain) :
	m_Player(player), m_Terrain(terrain)
{
	m_Handlers.emplace_back(PickUpHandler::create<HealthPack>(player, terrain, 10.0f, 1));
	m_Handlers.emplace_back(PickUpHandler::create<DoubleScore>(player, terrain, 10.0f, 1));
	m_Handlers.emplace_back(PickUpHandler::create<Bomb>(player, terrain, 10.0f, 1));
}

void PickUpManager::start()
{
	for (auto& h : m_Handlers)
	{
		h.play();
	}
}

void PickUpManager::pause()
{
	for (auto& h : m_Handlers)
	{
		h.pause();
	}
}

void PickUpManager::update(f32 dt)
{
	for (auto& h : m_Handlers)
	{
		h.update(dt);
	}
}

void PickUpManager::reset()
{
	for (auto& h : m_Handlers)
	{
		h.reset();
	}
}

void PickUpHandler::pause()
{
	m_Timer->pause();
}

void PickUpHandler::play()
{
	m_Timer->play();
}

void PickUpHandler::update(f32 dt)
{
	m_UpdateImpl(dt);
}

void PickUpHandler::reset()
{
	m_Timer->reset();
	m_ResetImpl();
}

PickUpHandler::~PickUpHandler()
{
	m_ResetImpl();
}

} // namespace game