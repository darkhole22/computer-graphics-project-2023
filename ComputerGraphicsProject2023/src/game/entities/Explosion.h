#pragma once

#include "vulture/util/String.h"
#include "vulture/scene/GameObject.h"

#include "Factory.h"
#include "CollisionMask.h"

using namespace vulture;

namespace game {

class Explosion
{
public:
	static const String s_ModelName;
	static const String s_TextureName;

	Ref<GameObject> m_GameObject;

	explicit Explosion(Ref<GameObject> gameObject);

	void setup(glm::vec3 initialPosition);

	EntityStatus update(float dt);

	~Explosion();
private:
	Ref<HitBox> m_Hitbox;
	EntityStatus m_Status;
};

} // namespace game