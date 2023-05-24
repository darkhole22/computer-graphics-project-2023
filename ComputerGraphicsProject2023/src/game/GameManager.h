#pragma once

#include <random>

#include "game/entities/Enemy.h"
#include "game/entities/Player.h"
#include "game/entities/Factory.h"
#include "vulture/core/Core.h"
#include "game/ui/HUD.h"
#include "game/terrain/Terrain.h"

namespace game {

class GameManager
{
public:
	explicit GameManager(Ref<Terrain> terrain);

	void update(float dt);

private:
	Scene* m_Scene = nullptr;
	Ref<Terrain> m_Terrain = nullptr;

	Ref<Player> m_Player = nullptr;

	Ref<HUD> m_HUD = nullptr;

	Factory<Enemy>* m_EnemyFactory;

};

} // namespace game