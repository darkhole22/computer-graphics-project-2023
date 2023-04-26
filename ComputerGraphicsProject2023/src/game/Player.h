#pragma once

#include <glm/vec3.hpp>
#include <utility>

#include "vulture/scene/GameObject.h"
#include "vulture/core/Core.h"
#include "vulture/core/Input.h"

using namespace vulture;

class Player
{
private:
	Ref<GameObject> m_GameObject;

	const float SPEED = 10;
	int hp = 10;

public:
	explicit Player(Ref<GameObject> gameObject) : m_GameObject(std::move(gameObject)) {}

	void update(float dt)
	{
		float x = Input::getAxis("MOVE_LEFT", "MOVE_RIGHT") * SPEED * dt;
		float z = Input::getAxis("MOVE_DOWN", "MOVE_UP") * SPEED * dt;

		glm::vec3 movement(x, 0.0f, z);
		if (glm::length(movement) > 1.f) {
			movement = glm::normalize(movement);
		}

		m_GameObject->translate(movement);
	}
};
