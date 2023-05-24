#pragma once

#include <utility>
#include <algorithm>

#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "vulture/core/Application.h"
#include "vulture/event//Event.h"
#include "vulture/scene/Camera.h"
#include "Factory.h"
#include "Bullet.h"
#include "game/terrain/Terrain.h"

using namespace vulture;

namespace game {

struct HealthUpdated
{
	int hp;
	int maxHp;
};

struct AmmoUpdated
{
	int ammo;
	int maxAmmo;
};

class Player
{
	EVENT(HealthUpdated)
	EVENT(AmmoUpdated)

private:
	const float c_Speed = 10;
	const float c_RotSpeed = 4.0f;

	Camera* m_Camera;
	float c_CameraHeight = 1.5f;

	Factory<Bullet>* m_BulletFactory;

public:
	Transform transform;

	int m_HP = 5;
	int m_MaxHP = 5;

	explicit Player();

	void update(float dt);
};

} // namespace game