#pragma once

#include <bitset>

#include "renderer/RenderTarget.h"

namespace computergraphicsproject {

class Scene
{
public:
	Scene();

	void render(RenderTarget target);
private:
	std::bitset<MAX_FRAMES_IN_FLIGHT> m_FrameModified;

	void recordCommandBuffer(RenderTarget& target);
};

} // namespace computergraphicsproject
