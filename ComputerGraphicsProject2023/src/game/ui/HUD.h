#pragma once

#include "game/EventBus.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

class HUD
{
public:
	HUD();

private:
	UIHandler* m_UIHandler;
	Window* m_Window;

	Ref<UIText> m_HPText;
	Ref<UIText> m_AmmoText;

	Ref<UIImage> m_Crosshair;

	Ref<UIText> m_PauseScreenTitleText;
	Ref<UIText> m_PauseScreenSubtitleText;

	Ref<UIText> m_GameOverText;

	void onHealthUpdated(HealthUpdated event);

	void onGameStateChanged(GameStateChanged event);

	template <class UIElement>
	void centerElement(Ref<UIElement> element, f32 xOffset = 0.0f, f32 yOffset = 0.0f)
	{
		element->setPosition((m_Window->getWidth() - element->getWidth()) / 2.0f + xOffset,
						  (m_Window->getHeight() - element->getHeight()) / 2.0f + yOffset);
	}
};

}