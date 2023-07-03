#pragma once

#include "vulture/scene/ui/UIHandler.h"
#include "vulture/core/Application.h"
#include "vulture/core/Input.h"

#include "game/EventBus.h"

namespace game {

using namespace vulture;

class DebugUI
{
public:
	DebugUI();

	void update(float dt);
private:
	UIHandler* m_UIHandler;
	Window* m_Window;

	bool m_Visible;

	Ref<UIText> m_FPSText;
	Ref<UIText> m_FrameTimeText;
	Ref<UIText> m_GodmodeText;

	void setTextPosition();
};

} // namespace game
