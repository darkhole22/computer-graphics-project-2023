#pragma once

#include "Factory.h"
#include "vulture/core/Application.h"
#include "CollisionMask.h"

namespace game {

class Bullet
{
public:
	static const String& s_ModelName;
	static const String& s_TextureName;

	Ref<GameObject> m_GameObject;
	Ref<HitBox> m_Hitbox;

	EntityStatus status;

	explicit Bullet(Ref<GameObject> gameObject);

	void setup(Transform startingTransform, glm::vec3 direction);

	EntityStatus update(float dt);

	~Bullet();

private:
	const float c_Speed = 30.0f;
	const float c_Range = 30.0f;

	glm::vec3 m_StartingPosition = glm::vec3(0.0f);
	glm::vec3 m_Direction = glm::vec3(1.0f, 0.0f, 0.0f);
};

}