#pragma once

#include "game/entities/Player.h"
#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

class HUD
{
public:
	explicit HUD(Ref<Player> player);

private:
	UIHandler* m_UIHandler;

	Ref<UIText> m_HPText;
	Ref<UIText> m_AmmoText;

	Ref<Player> m_Player;

	void onHealthUpdated(HealthUpdated event);
};

}