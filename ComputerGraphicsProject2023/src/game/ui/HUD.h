#pragma once

#include "game/EventBus.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

class HUD
{
public:
	HUD();

	void update(f32 dt);
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
};

}