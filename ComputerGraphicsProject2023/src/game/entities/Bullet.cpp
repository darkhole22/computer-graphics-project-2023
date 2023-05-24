#include "Bullet.h"

namespace game {

const String& Bullet::s_ModelName = "character";
const String& Bullet::s_TextureName = "character";

Bullet::Bullet(Ref<GameObject> gameObject) : m_GameObject(gameObject)
{
	m_GameObject->transform.setScale(0.7f);
}

void Bullet::setup(glm::vec3 startingPosition, glm::vec3 direction)
{
	m_StartingPosition = startingPosition + glm::vec3(0.0f, 0.8f, 0.0f);
	m_GameObject->transform.setPosition(m_StartingPosition);
	m_Direction = direction;
}

EntityStatus Bullet::update(float dt)
{
	m_GameObject->transform.translate(m_Direction * c_Speed * dt);

	if (glm::distance(m_StartingPosition, m_GameObject->transform.getPosition()) > c_Range)
	{
		return EntityStatus::DEAD;
	}

	return EntityStatus::ALIVE;
}

} // namespace game