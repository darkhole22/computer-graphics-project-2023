#include "HUD.h"

#include "vulture/core/Logger.h"

namespace game {

HUD::HUD()
{
	m_UIHandler = Application::getScene()->getUIHandle();

	m_Window = Application::getWindow();
	m_Window->addCallback([this](WindowResizedEvent event) {
		centerElement(m_PauseScreenTitle);
		centerElement(m_PauseScreenSubtitle, 0.0f, m_PauseScreenTitle->getHeight());

		centerElement(m_GameOverTitle);
		centerElement(m_GameOverSubtitle, 0.0f, m_GameOverTitle->getHeight());

		centerElement(m_LevelUpTitle, 0.0f, -30.0f);
		centerElement(m_LevelUpSubtitle, 0.0f, -30.0f + m_LevelUpTitle->getHeight());

		centerElement(m_Crosshair);
	});

	EventBus::addCallback([this](HealthUpdated e) { onHealthUpdated(e); });
	EventBus::addCallback([this](DashesUpdated e) { onDashesUpdated(e); });
	EventBus::addCallback([this](LevelUp e) { onLevelUp(e); });

	EventBus::addCallback([this](BulletShot e) { onBulletShot(e); });

	EventBus::addCallback([this](GameStateChanged e) { onGameStateChanged(e); });
	EventBus::addCallback([this](ScoreUpdated e) { onScoreUpdated(e); });

	m_HPText = m_UIHandler->makeText("HP: ");

	m_DashesText = m_UIHandler->makeText("Dashes: ");
	m_DashesText->setPosition({20, 50 });

	m_ScoreText = m_UIHandler->makeText("Score: ");
	m_ScoreText->setPosition({20, 80});

	m_Crosshair = m_UIHandler->makeImage("crosshair");
	m_Crosshair->setWidth(50);
	centerElement(m_Crosshair);

	/****************
	 * PAUSE SCREEN *
	 ****************/
	m_PauseScreenTitle = m_UIHandler->makeText("PAUSED");
	m_PauseScreenSubtitle = m_UIHandler->makeText("Press ESC to Resume.");

	centerElement(m_PauseScreenTitle);
	centerElement(m_PauseScreenSubtitle, 0.0f, m_PauseScreenTitle->getHeight());

	m_PauseScreenTitle->setStroke(0.6f);
	m_PauseScreenTitle->setVisible(false);
	m_PauseScreenSubtitle->setVisible(false);

	/********************
	 * GAME OVER SCREEN *
	 ********************/
	m_GameOverTitle = m_UIHandler->makeText("GAME OVER");
	m_GameOverSubtitle = m_UIHandler->makeText("Press R to Restart");

	centerElement(m_GameOverTitle);
	centerElement(m_GameOverSubtitle, 0.0f, m_GameOverTitle->getHeight());

	m_GameOverTitle->setStroke(0.6f);
	m_GameOverTitle->setVisible(false);
	m_GameOverSubtitle->setVisible(false);

	/********************
	 *     LEVEL UP     *
	 ********************/
	m_LevelUpTitle = m_UIHandler->makeText("LEVEL UP!");
	m_LevelUpSubtitle = m_UIHandler->makeText("Level Up Subtitle");

	centerElement(m_LevelUpTitle, 0.0f, -100.0f);
	centerElement(m_LevelUpSubtitle, 0.0f, -100.0f + m_LevelUpTitle->getHeight());

	m_LevelUpTitle->setStroke(0.6f);
	m_LevelUpTitle->setVisible(false);
	m_LevelUpSubtitle->setVisible(false);
}

void HUD::onHealthUpdated(HealthUpdated event)
{
	m_HPText->setText(stringFormat("HP: %d/%d", event.hp, event.maxHp));
}

void HUD::onDashesUpdated(DashesUpdated event)
{
	m_DashesText->setText(stringFormat("Dashes: %d/%d", event.dashes, event.maxDashes));
}

void HUD::onLevelUp(LevelUp event)
{
	m_LevelUpSubtitle->setText(event.message);

	m_LevelUpTitle->setVisible(true);
	m_LevelUpSubtitle->setVisible(true);

	auto tween = Application::getScene()->makeTween();
	tween->addIntervalTweener(1.0f);
	tween->addCallbackTweener([this] () {
		m_LevelUpTitle->setVisible(false);
		m_LevelUpSubtitle->setVisible(false);
	});
}

void HUD::onBulletShot(BulletShot event)
{
	auto tween = Application::getScene()->makeTween();

	std::function<void(f32)> cb = [this](f32 size) {
		m_Crosshair->setHeight(size);
		m_Crosshair->setWidth(size);
		centerElement(m_Crosshair);
	};

	auto initialSize = m_Crosshair->getHeight();
	auto finalSize = initialSize * 1.5f;

	tween->addMethodTweener(cb, initialSize, finalSize, 0.15f);
	tween->addMethodTweener(cb, finalSize, initialSize, 0.15f);
}


void HUD::onGameStateChanged(GameStateChanged event)
{
	m_PauseScreenTitle->setVisible(event.gameState == GameState::PAUSE);
	m_PauseScreenSubtitle->setVisible(event.gameState == GameState::PAUSE);

	m_GameOverTitle->setVisible(event.gameState == GameState::GAME_OVER);
	m_GameOverSubtitle->setVisible(event.gameState == GameState::GAME_OVER);
}

void HUD::onScoreUpdated(ScoreUpdated event)
{
	m_ScoreText->setText(stringFormat("Score: %d", event.score));
}


} // namespace game
