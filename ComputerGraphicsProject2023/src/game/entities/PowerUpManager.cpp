#include "PowerUpManager.h"

#include "game/entities/powerup/Bomb.h"
#include "game/entities/powerup/HealthPack.h"
#include "game/entities/powerup/DoubleScore.h"

namespace game {

PowerUpManager::PowerUpManager(Ref<Player> player, Ref<Terrain> terrain) :
	m_Player(player), m_Terrain(terrain)
{
	m_Handlers.emplace_back(PowerUpHandler::create<HealthPack>(player, terrain, 10.0f, 1));
	m_Handlers.emplace_back(PowerUpHandler::create<DoubleScore>(player, terrain, 10.0f, 1));
	m_Handlers.emplace_back(PowerUpHandler::create<Bomb>(player, terrain, 10.0f, 1));
}

void PowerUpManager::start()
{
	for (auto& h : m_Handlers)
	{
		h.play();
	}
}

void PowerUpManager::pause()
{
	for (auto& h : m_Handlers)
	{
		h.pause();
	}
}

void PowerUpManager::update(f32 dt)
{
	for (auto& h : m_Handlers)
	{
		h.update(dt);
	}
}

void PowerUpManager::reset()
{
	for (auto& h : m_Handlers)
	{
		h.reset();
	}
}

void PowerUpHandler::pause()
{
	m_Timer->pause();
}

void PowerUpHandler::play()
{
	m_Timer->play();
}

void PowerUpHandler::update(f32 dt)
{
	m_UpdateImpl(dt);
}

void PowerUpHandler::reset()
{
	m_Timer->reset();
	m_ResetImpl();
}

PowerUpHandler::~PowerUpHandler()
{
	m_ResetImpl();
}

} // namespace game