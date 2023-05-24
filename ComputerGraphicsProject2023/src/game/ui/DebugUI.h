#pragma once

#include "vulture/scene/ui/UIHandler.h"
#include "vulture/core/Application.h"
#include "vulture/core/Input.h"

using namespace vulture;

namespace game {

class DebugUI
{
public:
	DebugUI();

	void update(float dt);
private:
	UIHandler* m_HandlerUI = nullptr;

	Ref<UIText> m_FPSText;
	Ref<UIText> m_FrameTimeText;
};

} // namespace game
