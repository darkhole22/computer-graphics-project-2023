#pragma once

#include <utility>

#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "vulture/scene/Camera.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

class Bullet
{
private:
	const float c_Speed = 30.0f;
	const float c_Range = 60.0f;

	glm::vec3 m_StartingPosition = glm::vec3(0.0f);
	glm::vec3 m_Direction = glm::vec3(1.0f, 0.0f, 0.0f);

public:
	Ref<GameObject> m_GameObject;

	Bullet()
	{
		m_GameObject = makeRef<GameObject>("character");
		m_GameObject->transform.setScale(0.7f);
	}

	inline void setup(glm::vec3 startingPosition, glm::vec3 direction)
	{
		m_StartingPosition = startingPosition + glm::vec3(0.0f, 0.8f, 0.0f);
		m_GameObject->transform.setPosition(m_StartingPosition);
		m_Direction = direction;
	}

	inline void update(float dt)
	{
		m_GameObject->transform.translate(m_Direction * c_Speed * dt);

		if (glm::distance(m_StartingPosition, m_GameObject->transform.getPosition()) > c_Range)
		{
			Application::getScene()->removeObject(m_GameObject);
		}
	}

};

class Player
{
private:
	const float c_Speed = 10;
	const float c_RotSpeed = 4.0f;

	Camera* m_Camera;
	float c_CameraHeight = 1.5f;
public:
	Transform transform;

	std::vector<Bullet> m_Bullets;

	explicit Player()
	{
		m_Camera = Application::getScene()->getCamera();
		m_Camera->position = transform.getPosition() + glm::vec3(0.0f, c_CameraHeight, 0.0f);
	}

	void update(float dt);
};

} // namespace game