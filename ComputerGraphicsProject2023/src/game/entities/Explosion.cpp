#include "Explosion.h"

namespace game {

const String Explosion::s_ModelName = "explosion";
const String Explosion::s_TextureName = "grass";

Explosion::Explosion(Ref<GameObject> gameObject)
{
	m_GameObject = gameObject;

	m_Hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(1.0f, 2.0f));
	m_Hitbox->layerMask = PLAYER_BULLET_MASK;
	m_Hitbox->collisionMask = ENEMY_MASK;

	m_Hitbox->transform = m_GameObject->transform;
}

void Explosion::setup(glm::vec3 initialPosition)
{
	m_Status = EntityStatus::ALIVE;

	m_GameObject->transform->setPosition(initialPosition);

	auto scaleCallback = [this] (f32 scale) {
		m_GameObject->transform->setScale(scale);
	};

	auto tween = Application::getScene()->makeTween();
	tween->addMethodTweener<f32>(scaleCallback, 0.0f, 10.0f, 5.0f);
	tween->addIntervalTweener(3.0f);
	tween->addMethodTweener<f32>(scaleCallback, 5.0f, 0.0f, 5.0f);
	tween->addCallbackTweener([this] () { m_Status = EntityStatus::DEAD; });
}

EntityStatus Explosion::update(float dt)
{
	return m_Status;
}

Explosion::~Explosion()
{
	Application::getScene()->removeHitbox(m_Hitbox);
}

} // namespace game