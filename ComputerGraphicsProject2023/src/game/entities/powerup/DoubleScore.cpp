#include "DoubleScore.h"
#include "game/entities/CollisionMask.h"

namespace game {

const String DoubleScore::s_ModelName = "star";
const String DoubleScore::s_TextureName = "star";

DoubleScore::DoubleScore(Ref<GameObject> gameObject)
	: m_GameObject(gameObject)
{
	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(2.0f, 4.0f));

	m_Hitbox->layerMask = POWER_UP_MASK;
	m_Hitbox->collisionMask = PLAYER_MASK;

	m_Hitbox->transform = m_GameObject->transform;
	m_Hitbox->data = &m_Data;

	m_Hitbox->addCallback([this](const HitBoxEntered& e) {
		m_Status = EntityStatus::DEAD;
	});
}

void DoubleScore::setup(Ref<Terrain> terrain)
{
	m_Terrain = terrain;
	m_Status = EntityStatus::ALIVE;
	m_GameObject->transform->setScale(0.03f);

	Application::getScene()->addHitbox(m_Hitbox);
}

EntityStatus DoubleScore::update(f32 dt)
{
	m_DeltaHeight += dt;
	m_DeltaAngle += dt;

	auto pos = m_GameObject->transform->getPosition();
	m_GameObject->transform->setPosition(pos.x, m_Terrain->getHeightAt(pos.x, pos.z) + 1.0f + 0.5f * std::sin(m_DeltaHeight), pos.z);
	m_GameObject->transform->setRotation(0, m_DeltaAngle, 0);

	return m_Status;
}

DoubleScore::~DoubleScore()
{
	Application::getScene()->removeHitbox(m_Hitbox);
}

PowerUpType DoubleScoreData::getType() const
{
	return PowerUpType::DoubleScore;
}

f32 DoubleScoreData::getDuration() const
{
	return m_Duration;
}

} // namespace game
