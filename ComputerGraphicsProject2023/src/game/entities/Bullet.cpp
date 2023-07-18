#include "Bullet.h"

namespace game {

const String Bullet::s_ModelName = "bullet";
const String Bullet::s_TextureName = "bullet";
const String Bullet::s_EmissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME;
const String Bullet::s_RoughnessTextureName = "bullet";

Bullet::Bullet(Ref<GameObject> gameObject)
	: gameObject(gameObject)
{
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
	gameObject->transform = makeRef<Transform>(startingTransform);
	gameObject->transform->translate(0.0f, 1.5f, 0.0f);

	auto angle = std::sin(direction.y);
	gameObject->transform->rotate(0.0f, 0.0f, -glm::radians(90.0f) + angle);

	m_Status = maxHits > 0 ? EntityStatus::ALIVE : EntityStatus::DEAD;

	m_StartingPosition = startingTransform.getPosition();
	m_Direction = direction;

	m_HitsLeft = maxHits;

	m_Hitbox->transform = gameObject->transform;
	Application::getScene()->addHitbox(m_Hitbox);
}

EntityStatus Bullet::update(float dt)
{
	gameObject->transform->translate(m_Direction * c_Speed * dt);

	if (glm::distance(m_StartingPosition, gameObject->transform->getPosition()) > c_Range)
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