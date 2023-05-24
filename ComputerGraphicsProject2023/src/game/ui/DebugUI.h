#pragma once

#include "../../vulture/scene/ui/UIHandler.h"
#include "../../vulture/core/Application.h"
#include "../../vulture/core/Input.h"

using namespace vulture;

namespace game {

class DebugUI
{
private:
	UIHandler *m_HandlerUI = nullptr;

	Ref<UIText> m_FPSText;
	Ref<UIText> m_FrameTimeText;
public:
	DebugUI()
	{
		m_HandlerUI = Application::getScene()->getUIHandle();

		m_FPSText = m_HandlerUI->makeText("FPS");
		m_FrameTimeText = m_HandlerUI->makeText("Frame Time");
		m_FrameTimeText->setPosition({20, 50});

		m_FPSText->setVisible(false);
		m_FrameTimeText->setVisible(false);
	}

	void update(float dt)
	{
		if (Input::isActionJustPressed("TOGGLE_INFO"))
		{
			m_FPSText->setVisible(!m_FPSText->isVisible());
			m_FrameTimeText->setVisible(!m_FrameTimeText->isVisible());
		}

		static float fps = 0.0f;
		static float delta = 0;

		static const float WRITE_FPS_TIMEOUT = 0.5; // seconds
		static const float FPS_AVG_WEIGHT = 0.1f;    // 0 <= x <= 1

		delta += dt;
		fps = fps * (1.0f - FPS_AVG_WEIGHT) + (1.0f / dt) * FPS_AVG_WEIGHT;

		if (delta > WRITE_FPS_TIMEOUT) {
			m_FPSText->setText(stringFormat("FPS: %.0f", fps));
			m_FrameTimeText->setText(stringFormat("Frame time: %.4fms", dt * 1000));

			delta -= WRITE_FPS_TIMEOUT;
		}
	}

};

} // namespace game