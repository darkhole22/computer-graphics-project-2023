#pragma once

#include "Player.h"

namespace game {

class Enemy
{
public:
	Ref<GameObject> m_GameObject;


	Enemy(Ref<Player> player)
	{
		m_GameObject = makeRef<GameObject>("character");

		m_Player = player;
	}

	void update(float dt)
	{
		auto dir = glm::normalize(m_Player->transform.getPosition() - m_GameObject->transform.getPosition());
		m_GameObject->transform.translate(dir * c_Speed * dt);
	}
private:
	const float c_Speed = 4.0f;

	Ref<Player> m_Player;
};

} // namespace game