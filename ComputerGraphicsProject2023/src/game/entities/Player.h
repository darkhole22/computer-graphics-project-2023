#pragma once

#include <utility>
#include <algorithm>

#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "vulture/core/Application.h"
#include "vulture/event/Event.h"
#include "vulture/scene/Camera.h"

#include "CollisionMask.h"
#include "Factory.h"
#include "Bullet.h"
#include "game/terrain/Terrain.h"
#include "game/EventBus.h"

namespace game {

using namespace vulture;

class Player
{
public:
	Transform transform;
	Ref<HitBox> m_Hitbox;

	Player();

	void update(f32 dt);
private:
	const f32 c_Speed = 10;
	const f32 c_RotSpeed = 4.0f;
	const glm::vec2 c_MouseSensitivity = {0.7, -0.6};

	u32 m_HP = 5;
	u32 m_MaxHP = 5;

	bool m_Invincible = false;
	f32 m_InvincibilityDuration = 1.0f;

	Camera* m_Camera;
	f32 c_CameraHeight = 1.5f;

	Ref<Tween> m_FiringTween;
	const f32 c_FireRatio = 0.3f;

	Ref<Factory<Bullet>> m_BulletFactory;
};

} // namespace game