#include "Bullet.h"

namespace game {

const String& Bullet::s_ModelName = "bullet";
const String& Bullet::s_TextureName = "bullet";

Bullet::Bullet(Ref<GameObject> gameObject)
{
	m_GameObject = gameObject;

	// TODO: Those should be the correct values for the CollisionShape size
	// However they seem not to work properly: I suspect that's because of the rotation applied
	// to the bullet's GameObject.
	// We should look into this to get it fixed.
	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(0.6f * 0.2f, 3.8f * 0.2f));
	m_Hitbox->layerMask = PLAYER_BULLET_MASK;
}

void Bullet::setup(Transform startingTransform, glm::vec3 direction)
{
	m_GameObject->transform = makeRef<Transform>(startingTransform);
	m_GameObject->transform->translate(0.0f, 1.5f, 0.0f);
	m_GameObject->transform->setScale(0.2f);

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