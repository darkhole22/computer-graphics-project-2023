#include "HUD.h"

namespace game {

HUD::HUD() {
	m_UIHandler = Application::getScene()->getUIHandle();

	m_Window = Application::getWindow();
	m_Window->addCallback([this](WindowResizedEvent event) {
		m_PauseScreenTitleText->setPosition({ m_Window->getWidth() / 2 - 40, m_Window->getHeight() / 2 - 20});
		m_PauseScreenSubtitleText->setPosition({ m_Window->getWidth() / 2 - 140, m_Window->getHeight() / 2 });
		m_GameOverText->setPosition({ m_Window->getWidth() / 2 - 50, m_Window->getHeight() / 2 - 20 });
	});

	EventBus::addCallback([this](HealthUpdated e) { onHealthUpdated(e); });
	EventBus::addCallback([this](GameStateChanged e) { onGameStateChanged(e); });

	m_HPText = m_UIHandler->makeText("HP: ");
	m_AmmoText = m_UIHandler->makeText("Ammo: ");
	m_AmmoText->setPosition({20, 50});

	/****************
	 * PAUSE SCREEN *
	 ****************/
	m_PauseScreenTitleText = m_UIHandler->makeText("PAUSED");
	m_PauseScreenSubtitleText = m_UIHandler->makeText("Press ESC to Resume.");

	m_PauseScreenTitleText->setPosition({ m_Window->getWidth() / 2 - 40, m_Window->getHeight() / 2 - 20 });
	m_PauseScreenSubtitleText->setPosition({ m_Window->getWidth() / 2 - 140, m_Window->getHeight() / 2 });

	m_PauseScreenTitleText->setVisible(false);
	m_PauseScreenSubtitleText->setVisible(false);

	/********************
	 * GAME OVER SCREEN *
	 ********************/
	 m_GameOverText = m_UIHandler->makeText("GAME OVER");
	 m_GameOverText->setPosition({ m_Window->getWidth() / 2 - 50, m_Window->getHeight() / 2 - 20 });
	 m_GameOverText->setVisible(false);
}

void HUD::onHealthUpdated(HealthUpdated event)
{
	m_HPText->setText(stringFormat("HP: %d/%d", event.hp, event.maxHp));
}

void HUD::onGameStateChanged(GameStateChanged event)
{
	m_PauseScreenTitleText->setVisible(event.gameState == GameState::PAUSE);
	m_PauseScreenSubtitleText->setVisible(event.gameState == GameState::PAUSE);

	m_GameOverText->setVisible(event.gameState == GameState::GAME_OVER);
}

} // namespace game
