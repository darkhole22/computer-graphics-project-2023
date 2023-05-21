#pragma once

#include <utility>

#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "vulture/scene/Camera.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game
{
class Player
{
private:
	const float c_Speed = 10;
	const float c_RotSpeed = 4.0f;

	Camera* m_Camera;
	float c_CameraHeight = 1.5f;
public:
	Transform transform;

	explicit Player()
	{
		m_Camera = Application::getScene()->getCamera();
		m_Camera->position = transform.getPosition() + glm::vec3(0.0f, c_CameraHeight, 0.0f);
	}

	void update(float dt);
};

} // namespace game