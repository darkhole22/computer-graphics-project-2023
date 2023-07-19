#pragma once

#include "vulture/scene/Camera.h"
#include "vulture/audio/AudioPlayer.h"

#include "game/terrain/Terrain.h"
#include "game/components/MovementComponent.h"
#include "game/entities/Factory.h"
#include "game/entities/Bullet.h"
#include "game/entities/Explosion.h"

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
	explicit Player(Ref<Terrain> terrain);

	void update(f32 dt);

	void reset();

	inline glm::vec3 getPosition() const { return m_Transform->getPosition(); }

	inline bool getDoubleScoreActive() { return m_DoubleScoreActive; }

	~Player();
private:
	const f32 c_Speed = 10.0f;
	const f32 c_SlopeSpeed = 20.0f;
	const f32 c_MaxSlope = 0.6f;
	const f32 c_RotSpeed = 4.0f;
	f32 m_BobbingHeight = -0.4f;

	Ref<Terrain> m_Terrain;

	Camera* m_Camera;
	f32 m_CameraHeight = 1.5f;

	PlayerStats m_Stats;
	bool m_Invincible = false;
	f32 m_InvincibilityDuration = 1.0f;
	bool m_Godmode = false;

	Ref<Transform> m_Transform;
	Ref<MovementComponent> m_Movement;

	Ref<HitBox> m_Hitbox;
	Ref<HitBox> m_PickUpHitbox;

	Ref<Factory<Bullet>> m_BulletFactory;
	Ref<Tween> m_FiringTween;

	Factory<Explosion> m_ExplosionFactory;
	bool m_CanSpawnExplosion = true;

	bool m_DoubleScoreActive = false;

	AudioPlayer m_GunAudio;
	AudioPlayer m_DamageAudio;

	void updateFiringTween();

	void onHitBoxEntered(const HitBoxEntered& event);
	void onPickUpEntered(const HitBoxEntered& event);

	void onEnemyKilled(const EnemyDied& event);
};

} // namespace game