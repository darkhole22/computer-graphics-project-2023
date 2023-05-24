#include "HUD.h"

namespace game {

HUD::HUD()
{
	m_UIHandler = Application::getScene()->getUIHandle();

	EventBus::addCallback([this](HealthUpdated e) { onHealthUpdated(e); });

	m_HPText = m_UIHandler->makeText("HP: ");
	m_AmmoText = m_UIHandler->makeText("Ammo: ");
	m_AmmoText->setPosition({20, 50});
}

void HUD::onHealthUpdated(HealthUpdated event)
{
	m_HPText->setText(stringFormat("HP: %d/%d", event.hp, event.maxHp));
}

} // namespace game
