#include "Enemy.h"

namespace game {

const String& Enemy::s_ModelName = "character";
const String& Enemy::s_TextureName = "character";

Enemy::Enemy(Ref<GameObject> gameObject) : m_GameObject(gameObject)
{
	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.0f, 2.0f));

	m_Hitbox->layerMask = ENEMY_MASK;
	m_Hitbox->collisionMask = PLAYER_BULLET_MASK;

	m_Hitbox->addCallback([this](const HitBoxEntered& e) {
		m_Status = EntityStatus::DEAD;
	});
}

void Enemy::setup(Ref<Player> player)
{
	m_Player = player;
	m_Status = EntityStatus::ALIVE;

	m_Hitbox->transform = m_GameObject->transform;
	Application::getScene()->addHitbox(m_Hitbox);
}

EntityStatus Enemy::update(float dt)
{
	auto dir = m_Player->transform.getPosition() - m_GameObject->transform.getPosition();
	dir.y = 0;
	dir = glm::normalize(dir);

	m_GameObject->transform.translate(dir * c_Speed * dt);

	return m_Status;
}

Enemy::~Enemy()
{
	Application::getScene()->removeHitbox(m_Hitbox);
}

}