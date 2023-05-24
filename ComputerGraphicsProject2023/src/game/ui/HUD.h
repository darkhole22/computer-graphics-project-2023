#pragma once

#include "vulture/core/Application.h"

using namespace vulture;

namespace game {

class HUD
{
public:
	HUD(Ref<Player> player)
	{
		m_UIHandler = Application::getScene()->getUIHandle();

		m_Player = player;
		m_Player->addCallback([this](HealthUpdated e) { onHealthUpdated(e); });

		m_HPText = m_UIHandler->makeText(stringFormat("HP: %d/%d", player->m_HP, player->m_MaxHP));
		m_AmmoText = m_UIHandler->makeText("Ammo: ");
		m_AmmoText->setPosition({20, 50});
	}
private:
	UIHandler* m_UIHandler;

	Ref<UIText> m_HPText;
	Ref<UIText> m_AmmoText;

	Ref<Player> m_Player;

	void onHealthUpdated(HealthUpdated event)
	{
		m_HPText->setText(stringFormat("HP: %d/%d", event.hp, event.maxHp));
	}
};

}