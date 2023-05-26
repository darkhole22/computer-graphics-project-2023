#include "DebugUI.h"

using namespace vulture;

namespace game {

DebugUI::DebugUI()
{
	m_UIHandler = Application::getScene()->getUIHandle();

	m_FPSText = m_UIHandler->makeText("FPS");
	m_FrameTimeText = m_UIHandler->makeText("Frame Time");
	m_FrameTimeText->setPosition({ 20, 50 });

	m_FPSText->setVisible(false);
	m_FrameTimeText->setVisible(false);
}

void DebugUI::update(float dt)
{
	if (Input::isActionJustPressed("TOGGLE_INFO"))
	{
		m_FPSText->setVisible(!m_FPSText->isVisible());
		m_FrameTimeText->setVisible(!m_FrameTimeText->isVisible());
	}

	static float fps = 0.0f;
	static float delta = 0;

	static const float WRITE_FPS_TIMEOUT = 0.5; // seconds
	static const float FPS_AVG_WEIGHT = 0.1f;   // 0 <= x <= 1

	delta += dt;
	fps = fps * (1.0f - FPS_AVG_WEIGHT) + (1.0f / dt) * FPS_AVG_WEIGHT;

	if (delta > WRITE_FPS_TIMEOUT)
	{
		m_FPSText->setText(stringFormat("FPS: %.0f", fps));
		m_FrameTimeText->setText(stringFormat("Frame time: %.4fms", dt * 1000));

		delta -= WRITE_FPS_TIMEOUT;
	}
}

} // namespace game
