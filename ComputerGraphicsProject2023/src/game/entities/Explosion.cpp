#include "Explosion.h"

namespace game {

const String Explosion::s_ModelName = "explosion";
const String Explosion::s_TextureName = "explosion";
const String Explosion::s_EmissionTextureName = "explosion";
const String Explosion::s_RoughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME;

Explosion::Explosion(Ref<GameObject> gameObject)
	: m_Audio("explosion")
{
	m_GameObject = gameObject;
	m_GameObject->setEmissionStrength(1.0f);

	m_Capsule = makeRef<CapsuleCollisionShape>(1.0f, 2.0f);
	m_Hitbox = makeRef<HitBox>(m_Capsule);
	m_Hitbox->layerMask = EXPLOSION_MASK;
	m_Hitbox->collisionMask = ENEMY_MASK | PLAYER_MASK;
}

void Explosion::setup(glm::vec3 initialPosition)
{
	m_Status = EntityStatus::ALIVE;

	m_Hitbox->transform = m_GameObject->transform;
	m_GameObject->transform->setPosition(initialPosition);

	Application::getScene()->addHitbox(m_Hitbox);

	World* world = Application::getScene()->getWorld();

	auto scaleCallback = [this] (f32 scale) {
		m_Capsule->setDimensions(1.0f * scale, 2.0f * scale);
		m_GameObject->transform->setScale(scale);
	};

	auto rednessCallback = [world](f32 r) {
		world->pointLight.color = {r, 0.0f, 0.0f, 1.0f};
	};

	auto tween = Application::getScene()->makeTween();

	f32 finalScale = 10.0f;
	f32 finalRedness = 4.0f;

	f32 scaleUpDuration = 2.0f;
	f32 explosionDuration = 5.0f;
	f32 scaleDownDuration = 1.0f;

	// Scale Up
	auto scaleUpTween = tween->addParallelTweener();
	scaleUpTween->addMethodTweener<f32>(scaleCallback, 0.0f, finalScale, scaleUpDuration);
	scaleUpTween->addMethodTweener<f32>(rednessCallback, 0.0f, finalRedness, scaleUpDuration);

	tween->addIntervalTweener(explosionDuration);

	auto scaleDownTween = tween->addParallelTweener();
	scaleDownTween->addMethodTweener<f32>(scaleCallback, finalScale, 0.0f, scaleDownDuration);
	scaleDownTween->addMethodTweener<f32>(rednessCallback, finalRedness, 0.0f, scaleDownDuration);

	tween->addCallbackTweener([this] () { m_Status = EntityStatus::DEAD; });

	world->pointLight.position = m_GameObject->transform->getPosition();
	m_Audio.play();
	EventBus::emit(ExplosionStarted{});
}

EntityStatus Explosion::update(float dt)
{
	return m_Status;
}

Explosion::~Explosion()
{
	EventBus::emit(ExplosionFinished{});
	Application::getScene()->removeHitbox(m_Hitbox);
}

} // namespace game