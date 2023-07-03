#pragma once

#include "vulture/util/String.h"
#include "vulture/scene/GameObject.h"
#include "vulture/audio/AudioPlayer.h"

#include "game/entities/Factory.h"
#include "game/entities/CollisionMask.h"
#include "game/EventBus.h"
#include "game/events/GameEvents.h"

namespace game {

using namespace vulture;

class Explosion
{
public:
	static const String s_ModelName;
	static const String s_TextureName;
	static const String s_EmissionTextureName;
	static const String s_RoughnessTextureName;

	Ref<GameObject> m_GameObject;

	explicit Explosion(Ref<GameObject> gameObject);

	void setup(glm::vec3 initialPosition);

	EntityStatus update(float dt);

	~Explosion();
private:
	Ref<CapsuleCollisionShape> m_Capsule;
	Ref<HitBox> m_Hitbox;
	EntityStatus m_Status;

	AudioPlayer m_Audio;
};

} // namespace game