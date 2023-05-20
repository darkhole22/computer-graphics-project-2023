#pragma once

#include <utility>

#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "vulture/scene/Camera.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game
{
class Character
{
private:
	const float c_Speed = 10;
	const float c_RotSpeed = 4.0f;

	Camera* m_Camera;
public:
	explicit Character()
	{
		m_Camera = Application::getScene()->getCamera();
		m_Camera->position = glm::vec3(0.0f, 1.5f, 5.0f);
		m_Camera->setFarPlane(5000.0f);
	}

	void update(float dt);
};

} // namespace game