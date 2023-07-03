#pragma once

#include "Player.h"
#include "Factory.h"

namespace game {

using namespace vulture;

class Enemy
{
public:
	static const String s_ModelName;
	static const String s_TextureName;
	static const String s_EmissionTextureName;
	static const String s_RoughnessTextureName;


	Ref<GameObject> m_GameObject;
	Ref<HitBox> m_Hitbox;

	explicit Enemy(Ref<GameObject> gameObject);

	void setup(Ref<Player> player, Ref<Terrain> terrain, glm::vec3 spawnLocation);

	EntityStatus update(f32 dt);

	inline f32 getFlyingHeight() { return m_FlyingHeight; }

	~Enemy();

private:
	const f32 c_Speed = 4.0f;

	EntityStatus m_Status = EntityStatus::ALIVE;
	f32 m_FlyingHeight = 1.0f;

	Ref<MovementComponent> m_Movement;

	Ref<Player> m_Player;
	Ref<Terrain> m_Terrain;

	Ref<Tween> m_BobbingTween;

	u32 m_Damage = 1;
};

} // namespace game