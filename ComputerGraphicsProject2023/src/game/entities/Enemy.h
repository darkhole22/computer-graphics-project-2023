#pragma once

#include "Player.h"
#include "Factory.h"

namespace game {

class Enemy
{
public:
	static const String s_ModelName;
	static const String s_TextureName;
	static const String s_EmissionTextureName;
	static const String s_RoughnessTextureName;

	static constexpr f32 s_FlyingHeight = 1.0f;

	Ref<GameObject> m_GameObject;
	Ref<HitBox> m_Hitbox;

	explicit Enemy(Ref<GameObject> gameObject);

	void setup(Ref<Player> player, glm::vec3 spawnLocation);

	EntityStatus update(float dt);

	~Enemy();

private:
	const float c_Speed = 4.0f;

	EntityStatus m_Status = EntityStatus::ALIVE;

	Ref<MovementComponent> m_Movement;

	Ref<Player> m_Player;

	u32 m_Damage = 1;
};

} // namespace game