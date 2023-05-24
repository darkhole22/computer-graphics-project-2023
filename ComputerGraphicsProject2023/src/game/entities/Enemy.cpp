#include "Enemy.h"

namespace game {

const String& Enemy::s_ModelName = "character";
const String& Enemy::s_TextureName = "character";

Enemy::Enemy(Ref<GameObject> gameObject) : m_GameObject(gameObject) {}

void Enemy::setup(Ref<Player> player) { m_Player = player; }

EntityStatus Enemy::update(float dt)
{
	auto collidingBullets = Application::getScene()->getCollidingObjects(m_GameObject->transform, "PLAYER_BULLET");
	if (!collidingBullets.empty())
	{
		return EntityStatus::DEAD;
	}

	auto dir = m_Player->transform.getPosition() - m_GameObject->transform.getPosition();
	dir.y = 0;
	dir = glm::normalize(dir);

	m_GameObject->transform.translate(dir * c_Speed * dt);

	return EntityStatus::ALIVE;
}

}