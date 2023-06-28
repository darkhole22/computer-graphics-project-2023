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
#include "game/components/MovementComponent.h"
#include "vulture/util/Random.h"
#include "vulture/audio/AudioPlayer.h"
#include "Explosion.h"

namespace game {

using namespace vulture;

struct PlayerStats
{
	u32 hp = c_StartingHP;
	u32 maxHp = c_StartingHP;

	u32 maxBulletHits = c_StartingMaxBulletHits;

	u32 dashesLeft = c_StartingDashes;
	u32 maxDashes = c_StartingDashes;
	f32 dashCooldown = c_StartingDashCooldown;
	f32 dashDuration = 0.15f;
	f32 dashSpeed = 1.0f;
	f32 maxDashSpeed = 20.0f;

	f32 fireCooldown = c_StartingFireCooldown;

	u32 exp = 0;
	u32 level = 1;

	static constexpr u32 c_StartingHP = 5;

	static constexpr u32 c_StartingMaxBulletHits = 1;

	static constexpr u32 c_ExpRequired = 10;

	static constexpr u32 c_StartingDashes = 1;
	static constexpr f32 c_StartingDashCooldown = 3.0f;

	static constexpr f32 c_StartingFireCooldown = 1.0f;
};

class Player
{
public:
	Ref<Transform> transform;
	Ref<HitBox> m_Hitbox;
	Ref<HitBox> m_PowerUpHitbox;

	Player(Ref<Terrain> terrain);

	void update(f32 dt);

	void reset();
private:
	const f32 c_Speed = 10.0f;
	const f32 c_SlopeSpeed = 20.0f;
	const f32 c_MaxSlope = 0.6f;
	const f32 c_RotSpeed = 4.0f;

	Ref<MovementComponent> m_Movement;

	PlayerStats m_Stats;

	bool m_Godmode = false;

	bool m_Invincible = false;
	f32 m_InvincibilityDuration = 1.0f;

	Ref<Terrain> m_Terrain;

	Camera* m_Camera;
	f32 c_CameraHeight = 1.5f;

	Ref<Tween> m_FiringTween;
	f32 m_BobbingHeight = 1.0f;

	Ref<Factory<Bullet>> m_BulletFactory;
	AudioPlayer m_GunAudio;
	AudioPlayer m_DamageAudio;

	Ref<Factory<Explosion>> m_ExplosionFactory;

	void updateFiringTween();

	void onHitBoxEntered(const HitBoxEntered& e);
	void onEnemyKilled(const EnemyDied& event);
};

} // namespace game