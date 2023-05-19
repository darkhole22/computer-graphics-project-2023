#pragma once

namespace game {

class Enemy
{
public:
	Ref<GameObject> m_GameObject;

	Enemy()
	{
		m_GameObject = makeRef<GameObject>("character");
	}

	void update(float dt)
	{
		auto playerPos = Application::getScene()->getCamera()->position;
		playerPos.y = 0;

		auto dir = glm::normalize(playerPos - m_GameObject->getPosition());

		VUINFO("%f %f %f", dir.x, dir.y, dir.z);
		m_GameObject->translate(dir * c_Speed * dt);
	}
private:
	const float c_Speed = 4.0f;
};

} // namespace game