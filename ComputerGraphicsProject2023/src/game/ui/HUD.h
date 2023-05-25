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

	Ref<UIText> m_HPText;
	Ref<UIText> m_AmmoText;

	Ref<UIText> m_PauseScreenTitleText;
	Ref<UIText> m_PauseScreenSubtitleText;

	void onHealthUpdated(HealthUpdated event);

	void onGameStateChanged(GameStateChanged event);
};

}