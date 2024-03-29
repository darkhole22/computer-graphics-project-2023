#include "Enemy.h"

namespace game {

const String Enemy::s_ModelName = "hand-robot";
const String Enemy::s_TextureName = "hand-robot";
const String Enemy::s_EmissionTextureName = "hand-robot"; // DEFAULT_EMISSION_TEXTURE_NAME;
const String Enemy::s_RoughnessTextureName = "hand-robot";

Enemy::Enemy(Ref<GameObject> gameObject) : gameObject(gameObject)
{
	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(0.9f, 1.8f));
	gameObject->transform->setScale(0.5f);
	gameObject->setEmissionStrength(1.0f);

	m_Movement = makeRef<MovementComponent>(gameObject->transform);

	m_Hitbox->layerMask = ENEMY_MASK;
	m_Hitbox->collisionMask = PLAYER_BULLET_MASK | EXPLOSION_MASK;
	m_Hitbox->data = &m_Damage;
	m_Hitbox->transform = gameObject->transform;

	m_Hitbox->addCallback([this](const HitBoxEntered& e) {
		EventBus::emit(EnemyDied{});
		m_Status = EntityStatus::DEAD;
	});

	m_BobbingTween = Application::getScene()->makeTween();
	m_BobbingTween->loop();
	m_BobbingTween->addMethodTweener<f32>([this](f32 val) {
		m_FlyingHeight = 1.0f + 0.2f * std::sin(val);
	}, 0.0f, glm::radians(360.0f), 1.0f);
}

void Enemy::setup(Ref<Player> player, Ref<Terrain> terrain, glm::vec3 spawnLocation)
{
	m_Player = player;
	m_Status = EntityStatus::ALIVE;

	m_Terrain = terrain;

	gameObject->transform->setPosition(spawnLocation);

	Application::getScene()->addHitbox(m_Hitbox);
}

EntityStatus Enemy::update(f32 dt)
{
	auto target = m_Player->getPosition();
	m_Movement->lookAt({ target.x, gameObject->transform->getPosition().y, target.z });
	m_Movement->move(c_Speed * dt, 0, 0);

	auto pos = gameObject->transform->getPosition();
	gameObject->transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + getFlyingHeight(), pos.z);

	return m_Status;
}

Enemy::~Enemy()
{
	m_BobbingTween->stop();
	Application::getScene()->removeHitbox(m_Hitbox);
}

}