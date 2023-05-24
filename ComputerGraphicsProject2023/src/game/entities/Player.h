#pragma once

#include <utility>
#include <algorithm>

#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "vulture/core/Application.h"
#include "vulture/event/Event.h"
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

public:
	Transform transform;

	int m_HP = 5;
	int m_MaxHP = 5;

	Player();

	void update(float dt);
private:
	const float c_Speed = 10;
	const float c_RotSpeed = 4.0f;

	Camera* m_Camera;
	float c_CameraHeight = 1.5f;

	Ref<Factory<Bullet>> m_BulletFactory;

	bool m_Invincible = false;
	f32 m_InvincibilityDuration = 1.0f;
};

} // namespace game