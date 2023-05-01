//
// Created by michele on 4/12/23.
//

#include "Volcano.h"
#include "vulture/core/Application.h"

using namespace vulture;

void Volcano::update(float dt)
{
	float x = Input::getAxis("MOVE_LEFT", "MOVE_RIGHT") * SPEED * dt;
	float z = Input::getAxis("MOVE_DOWN", "MOVE_UP") * SPEED * dt;

	glm::vec3 movement(x, 0.0f, z);
	if (glm::length(movement) > 1.f) {
		movement = glm::normalize(movement);
	}

	m_GameObject->translate(movement);

	{
		static bool toggleDelete = true;
		static float lastClick = 0.0f;
		static float time = 0.0f;

		time += dt;

		if (Input::isKeyPressed(GLFW_KEY_U)) {
			if (time - lastClick > 0.3f) {
				auto s = Application::getScene();

				lastClick = time;
				toggleDelete ? s->removeObject(m_GameObject) : s->addObject(m_GameObject);
				toggleDelete = !toggleDelete;
			}
		}
	}
}