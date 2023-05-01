#pragma once

#include <glm/vec3.hpp>
#include <utility>

#include "vulture/scene/GameObject.h"
#include "vulture/core/Core.h"
#include "vulture/core/Input.h"

using namespace vulture;

class Volcano
{
private:
	const float SPEED = 10;
	int hp = 10;

public:
	Ref<GameObject> m_GameObject;

	explicit Volcano(Ref<GameObject> gameObject) : m_GameObject(std::move(gameObject)) {}

	void update(float dt);
};
