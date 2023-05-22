#pragma once

#include <utility>

#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "vulture/scene/Camera.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {
class Character
{
private:
	const float c_Speed = 10;
	const float c_RotSpeed = 4.0f;

	Camera* m_Camera;
	glm::vec3 m_Position;

	float c_CameraHeight = 1.5f;
public:
	explicit Character()
	{
		m_Camera = Application::getScene()->getCamera();
		m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Camera->position = m_Position + glm::vec3(0.0f, c_CameraHeight, 0.0f);
		m_Camera->setFarPlane(5000.0f);
	}

	void update(float dt);
};

} // namespace game