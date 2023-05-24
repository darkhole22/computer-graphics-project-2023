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

	void onHealthUpdated(HealthUpdated event);
};

}