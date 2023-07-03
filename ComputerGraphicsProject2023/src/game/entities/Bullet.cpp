#include "Bullet.h"

namespace game {

const String Bullet::s_ModelName = "bullet";
const String Bullet::s_TextureName = "bullet"; // TODO The texture seems to be broken
const String Bullet::s_EmissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME;
const String Bullet::s_RoughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME;

Bullet::Bullet(Ref<GameObject> gameObject)
{
	m_GameObject = gameObject;

	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(0.6f, 3.6f));
	m_Hitbox->layerMask = PLAYER_BULLET_MASK;
	m_Hitbox->collisionMask = ENEMY_MASK;

	m_Hitbox->addCallback([this](const HitBoxEntered& e) {
		m_HitsLeft--;

		if (m_HitsLeft <= 0)
		{
			m_Status = EntityStatus::DEAD;
		}
	});
}

void Bullet::setup(Transform startingTransform, glm::vec3 direction, u32 maxHits)
{
	m_GameObject->transform = makeRef<Transform>(startingTransform);
	m_GameObject->transform->translate(0.0f, 1.5f, 0.0f);
	m_GameObject->transform->setScale(0.1f);

	auto angle = std::sin(direction.y);
	m_GameObject->transform->rotate(0.0f, 0.0f, -glm::radians(90.0f) + angle);

	m_Status = maxHits > 0 ? EntityStatus::ALIVE : EntityStatus::DEAD;

	m_StartingPosition = startingTransform.getPosition();
	m_Direction = direction;

	m_HitsLeft = maxHits;

	m_Hitbox->transform = m_GameObject->transform;
	Application::getScene()->addHitbox(m_Hitbox);
}

EntityStatus Bullet::update(float dt)
{
	m_GameObject->transform->translate(m_Direction * c_Speed * dt);

	if (glm::distance(m_StartingPosition, m_GameObject->transform->getPosition()) > c_Range)
	{
		return EntityStatus::DEAD;
	}

	return m_Status;
}

Bullet::~Bullet()
{
	Application::getScene()->removeHitbox(m_Hitbox);
}

} // namespace game