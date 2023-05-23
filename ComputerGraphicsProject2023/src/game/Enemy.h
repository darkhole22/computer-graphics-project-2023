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

	Enemy()
	{
		m_GameObject = makeRef<GameObject>("character");
	}

	Enemy(Ref<GameObject> gameObject) : m_GameObject(gameObject) {}

	void setup(Ref<Player> player) { m_Player = player; }

	EntityStatus update(float dt)
	{
		m_Lifetime += dt;

		if (m_Lifetime >= 7.0f)
		{
			return EntityStatus::DEAD;
		}

		auto dir = m_Player->transform.getPosition() - m_GameObject->transform.getPosition();
		dir.y = 0;
		dir = glm::normalize(dir);

		m_GameObject->transform.translate(dir * c_Speed * dt);

		return EntityStatus::ALIVE;
	}
private:
	const float c_Speed = 4.0f;

	float m_Lifetime = 0.0f;

	Ref<Player> m_Player;
};

} // namespace game