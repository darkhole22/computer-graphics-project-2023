#pragma once

#include "Player.h"
#include "Factory.h"

namespace game {

class Enemy
{
public:
	static const String& s_ModelName;
	static const String& s_TextureName;

	Ref<GameObject> m_GameObject;
	Ref<HitBox> m_Hitbox;

	explicit Enemy(Ref<GameObject> gameObject);

	void setup(Ref<Player> player);

	EntityStatus update(float dt);

private:
	const float c_Speed = 4.0f;
	Ref<Player> m_Player;
	EntityStatus m_Status = EntityStatus::ALIVE;
};

} // namespace game