#include "Enemy.h"

namespace game {

const String Enemy::s_ModelName = "hand-robot";
const String Enemy::s_TextureName = "hand-robot";
const String Enemy::s_EmissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME;
const String Enemy::s_RoughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME;

Enemy::Enemy(Ref<GameObject> gameObject) : m_GameObject(gameObject)
{
	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(0.9f, 1.8f));
	m_GameObject->transform->setScale(0.5f);

	m_Movement = makeRef<MovementComponent>(m_GameObject->transform);

	m_Hitbox->layerMask = ENEMY_MASK;
	m_Hitbox->collisionMask = PLAYER_BULLET_MASK | EXPLOSION_MASK;
	m_Hitbox->data = &m_Damage;
	m_Hitbox->transform = m_GameObject->transform;

	m_Hitbox->addCallback([this](const HitBoxEntered& e) {
		EventBus::emit(EnemyDied{});
		m_Status = EntityStatus::DEAD;
	});
}

void Enemy::setup(Ref<Player> player, glm::vec3 spawnLocation)
{
	m_Player = player;
	m_Status = EntityStatus::ALIVE;

	m_GameObject->transform->setPosition(spawnLocation);

	Application::getScene()->addHitbox(m_Hitbox);
}

EntityStatus Enemy::update(float dt)
{
	auto target = m_Player->transform->getPosition();
	m_Movement->lookAt({target.x, m_GameObject->transform->getPosition().y, target.z});
	m_Movement->move(c_Speed * dt, 0, 0);

	return m_Status;
}

Enemy::~Enemy()
{
	Application::getScene()->removeHitbox(m_Hitbox);
}

}