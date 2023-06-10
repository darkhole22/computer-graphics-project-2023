#include "Bullet.h"

namespace game {

const String& Bullet::s_ModelName = "bullet";
const String& Bullet::s_TextureName = "bullet"; // TODO The texture seems to be broken

Bullet::Bullet(Ref<GameObject> gameObject)
{
	m_GameObject = gameObject;

	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(0.6f, 3.6f));
	m_Hitbox->layerMask = PLAYER_BULLET_MASK;
}

void Bullet::setup(Transform startingTransform, glm::vec3 direction)
{
	m_GameObject->transform = makeRef<Transform>(startingTransform);
	m_GameObject->transform->translate(0.0f, 1.5f, 0.0f);
	m_GameObject->transform->setScale(0.1f);

	m_StartingPosition = startingTransform.getPosition();

	auto angle = sin(direction.y);
	m_GameObject->transform->rotate(0.0f, 0.0f, -glm::radians(90.0f) + angle);

	m_Direction = direction;

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

	return EntityStatus::ALIVE;
}

Bullet::~Bullet()
{
	Application::getScene()->removeHitbox(m_Hitbox);
}

} // namespace game