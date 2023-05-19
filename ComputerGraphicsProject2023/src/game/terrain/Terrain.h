#pragma once
#include "vulture/core/Application.h"
#include "vulture/scene/Scene.h"

namespace game {

class Terrain
{
public:
	Terrain();

	void update(float dt);
private:
	vulture::Scene* scene = nullptr;
};

} // namespace game
