#include "HUD.h"

#include "vulture/core/Logger.h"

namespace game {

HUD::HUD()
{
	m_UIHandler = Application::getScene()->getUIHandle();

	m_Window = Application::getWindow();
	m_Window->addCallback([this](WindowResizedEvent event) {
		centerElement(m_PauseScreenTitleText);
		centerElement(m_PauseScreenSubtitleText, 0.0f, m_PauseScreenTitleText->getHeight());
		centerElement(m_GameOverTitleText);
		centerElement(m_GameOverSubtitleText, 0.0f, m_GameOverTitleText->getHeight());

		centerElement(m_Crosshair);
	});

	EventBus::addCallback([this](HealthUpdated e) { onHealthUpdated(e); });
	EventBus::addCallback([this](GameStateChanged e) { onGameStateChanged(e); });

	m_HPText = m_UIHandler->makeText("HP: ");
	m_AmmoText = m_UIHandler->makeText("Ammo: ");
	m_AmmoText->setPosition({ 20, 50 });

	m_Crosshair = m_UIHandler->makeImage("crosshair");
	m_Crosshair->setWidth(50);
	centerElement(m_Crosshair);

	/****************
	 * PAUSE SCREEN *
	 ****************/
	m_PauseScreenTitleText = m_UIHandler->makeText("PAUSED");
	m_PauseScreenSubtitleText = m_UIHandler->makeText("Press ESC to Resume.");

	centerElement(m_PauseScreenTitleText);
	centerElement(m_PauseScreenSubtitleText, 0.0f, m_PauseScreenTitleText->getHeight());

	m_PauseScreenTitleText->setStroke(0.6f);
	m_PauseScreenTitleText->setVisible(false);
	m_PauseScreenSubtitleText->setVisible(false);

	/********************
	 * GAME OVER SCREEN *
	 ********************/
	m_GameOverTitleText = m_UIHandler->makeText("GAME OVER");
	m_GameOverSubtitleText = m_UIHandler->makeText("Press R to Restart");

	centerElement(m_GameOverTitleText);
	centerElement(m_GameOverSubtitleText, 0.0f, m_GameOverTitleText->getHeight());

	m_GameOverTitleText->setStroke(0.6f);
	m_GameOverTitleText->setVisible(false);
	m_GameOverSubtitleText->setVisible(false);
}

void HUD::onHealthUpdated(HealthUpdated event)
{
	m_HPText->setText(stringFormat("HP: %d/%d", event.hp, event.maxHp));
}

void HUD::onGameStateChanged(GameStateChanged event)
{
	m_PauseScreenTitleText->setVisible(event.gameState == GameState::PAUSE);
	m_PauseScreenSubtitleText->setVisible(event.gameState == GameState::PAUSE);

	m_GameOverTitleText->setVisible(event.gameState == GameState::GAME_OVER);
	m_GameOverSubtitleText->setVisible(event.gameState == GameState::GAME_OVER);
}

} // namespace game
