#include "HealthPack.h"
#include "game/entities/CollisionMask.h"

namespace game {

const String HealthPack::s_ModelName = "health";
const String HealthPack::s_TextureName = "health";

HealthPack::HealthPack(Ref<GameObject> gameObject)
	: m_GameObject(gameObject)
{
	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.0f, 2.0f));

	m_Hitbox->layerMask = POWER_UP_MASK;
	m_Hitbox->collisionMask = PLAYER_MASK;

	m_Hitbox->data = &m_Data;

	m_Hitbox->addCallback([this](const HitBoxEntered& e) {
		m_Status = EntityStatus::DEAD;
	});
}

void HealthPack::setup(Ref<Terrain> terrain)
{
	m_Terrain = terrain;
	m_Status = EntityStatus::ALIVE;
	m_GameObject->transform.setScale(0.5f);

	m_Hitbox->transform = m_GameObject->transform;
	Application::getScene()->addHitbox(m_Hitbox);
}

EntityStatus HealthPack::update(f32 dt)
{
	m_DeltaHeight += dt;
	m_DeltaAngle += dt;

	auto pos = m_GameObject->transform.getPosition();
	m_GameObject->transform.setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + 1.0f + 0.5f * std::sin(m_DeltaHeight), pos.z);
	m_GameObject->transform.setRotation(0, m_DeltaAngle, 0);
	m_Hitbox->transform = m_GameObject->transform;
	m_Hitbox->transform.setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + 1.0f, pos.z);

	return m_Status;
}

HealthPack::~HealthPack()
{
	Application::getScene()->removeHitbox(m_Hitbox);
}

PowerUpType HealthPackData::getType() const
{
	return PowerUpType::HealthUp;
}

u32 HealthPackData::getHealth() const
{
	return m_Health;
}

} // namespace game
