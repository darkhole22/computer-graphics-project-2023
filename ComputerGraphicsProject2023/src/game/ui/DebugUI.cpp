#include "DebugUI.h"

using namespace vulture;

namespace game {

DebugUI::DebugUI()
{
	m_UIHandler = Application::getScene()->getUIHandle();

	m_Window = Application::getWindow();
	m_Window->addCallback([this](WindowResizedEvent event) {
		setTextPosition();
	});

	m_FPSText = m_UIHandler->makeText("FPS");
	m_FrameTimeText = m_UIHandler->makeText("FT");

	m_FPSText->setColor(0.0f, 0.0f, 0.0f);
	m_FrameTimeText->setColor(0.0f, 0.0f, 0.0f);

	m_FPSText->setVisible(false);
	m_FrameTimeText->setVisible(false);

	setTextPosition();
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
		m_FrameTimeText->setText(stringFormat("FT: %.3fms", dt * 1000));

		delta -= WRITE_FPS_TIMEOUT;
	}
}

void DebugUI::setTextPosition()
{
	f32 rightOffset = 250.0f;
	f32 topOffset = 20.0f;

	m_FPSText->setPosition(m_Window->getWidth() - rightOffset, topOffset);
	m_FrameTimeText->setPosition(m_Window->getWidth() - rightOffset,m_FPSText->getHeight() + topOffset);
}

} // namespace game
