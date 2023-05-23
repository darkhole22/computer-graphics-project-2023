#pragma once

#include <utility>

#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "vulture/scene/Camera.h"
#include "vulture/core/Application.h"
#include "Factory.h"
#include "Bullet.h"

using namespace vulture;

namespace game {

class Player
{
private:
	const float c_Speed = 10;
	const float c_RotSpeed = 4.0f;

	Camera* m_Camera;
	float c_CameraHeight = 1.5f;

	Factory<Bullet>* m_BulletFactory;

public:
	Transform transform;

	explicit Player();

	void update(float dt);
};

} // namespace game