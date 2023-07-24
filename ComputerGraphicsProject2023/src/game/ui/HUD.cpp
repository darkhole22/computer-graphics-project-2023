#include "HUD.h"

#include "vulture/core/Logger.h"

namespace game {

void setupStatsTextStyle(Ref<UIText> text);
void setupTitleStyle(Ref<UIText> text);
void setupSubtitleStyle(Ref<UIText> text);

HUD::HUD()
{
	m_UIHandler = Application::getScene()->getUIHandle();

	m_Window = Application::getWindow();
	m_Window->addCallback([this](WindowResizedEvent event) {
		centerElement(m_PauseScreenTitle);
		centerElement(m_PauseScreenSubtitle, 0.0f, m_PauseScreenTitle->getHeight());

		centerElement(m_GameOverTitle);
		centerElement(m_GameOverSubtitle, 0.0f, m_GameOverTitle->getHeight());

		centerElement(m_NotificationTitle, 0.0f, -200.0f);
		centerElement(m_NotificationSubtitle, 0.0f, -200.0f + m_NotificationTitle->getHeight());

		centerElement(m_Crosshair);

		if (m_LoadingScreenTitle) centerElement(m_LoadingScreenTitle);
	});

	EventBus::addCallback([this](HealthUpdated e) { onHealthUpdated(e); });
	EventBus::addCallback([this](DashesUpdated e) { onDashesUpdated(e); });
	EventBus::addCallback([this](LevelUp e) { onLevelUp(e); });

	EventBus::addCallback([this](BulletShot e) { onBulletShot(e); });

	EventBus::addCallback([this](GameStateChanged e) { onGameStateChanged(e); });
	EventBus::addCallback([this](ScoreUpdated e) { onScoreUpdated(e); });
	EventBus::addCallback([this](DoubleScoreStarted e) { onDoubleScoreStarted(e); });
	EventBus::addCallback([this](DoubleScoreOver e) { onDoubleScoreOver(e); });

	m_LoadingScreenTitle = m_UIHandler->makeText("Loading...");
	m_LoadingScreenTitle->setSize(30);
	centerElement(m_LoadingScreenTitle);

	m_HPText = m_UIHandler->makeText("HP: ");
	setupStatsTextStyle(m_HPText);

	m_DashesText = m_UIHandler->makeText("Dashes: ");
	m_DashesText->setPosition({ 20, 50 });
	setupStatsTextStyle(m_DashesText);

	m_ScoreText = m_UIHandler->makeText("Score: ");
	m_ScoreText->setPosition({ 20, 80 });
	setupStatsTextStyle(m_ScoreText);

	m_Crosshair = m_UIHandler->makeImage("crosshair");
	m_Crosshair->setWidth(50);
	centerElement(m_Crosshair);
	m_Crosshair->setVisible(false);

	/****************
	 * TITLE SCREEN *
	 ****************/
	m_TitleScreenTitle = m_UIHandler->makeText("ROBOT SURVIVOR");
	m_TitleScreenSubtitle = m_UIHandler->makeText("Press 'FIRE' to start.");

	m_TitleScreenTitle->setSize(30);
	setupTitleStyle(m_TitleScreenTitle);
	setupSubtitleStyle(m_TitleScreenSubtitle);

	centerElement(m_TitleScreenTitle);
	centerElement(m_TitleScreenSubtitle, 0.0f, m_TitleScreenTitle->getHeight());

	auto scaleCallback = [this] (f32 scale) {
		m_TitleScreenTitle->setSize(scale);
		m_TitleScreenSubtitle->setSize(scale * 0.8f);
		centerElement(m_TitleScreenTitle);
		centerElement(m_TitleScreenSubtitle, 0.0f, m_TitleScreenTitle->getHeight());
	};

	m_TitleTween = Application::getScene()->makeTween();
	m_TitleTween->loop();
	m_TitleTween->addMethodTweener<f32>(scaleCallback, 25.0f, 40.0f, 1.0f);
	m_TitleTween->addMethodTweener<f32>(scaleCallback, 40.0f, 25.0f, 1.0f);

	/****************
	 * PAUSE SCREEN *
	 ****************/
	m_PauseScreenTitle = m_UIHandler->makeText("PAUSED");
	m_PauseScreenSubtitle = m_UIHandler->makeText("ESC/START to Resume");

	setupTitleStyle(m_PauseScreenTitle);
	setupSubtitleStyle(m_PauseScreenSubtitle);

	centerElement(m_PauseScreenTitle);
	centerElement(m_PauseScreenSubtitle, 0.0f, m_PauseScreenTitle->getHeight());

	/********************
	 * GAME OVER SCREEN *
	 ********************/
	m_GameOverTitle = m_UIHandler->makeText("GAME OVER");
	m_GameOverSubtitle = m_UIHandler->makeText("R/START to Restart");

	setupTitleStyle(m_GameOverTitle);
	setupSubtitleStyle(m_GameOverSubtitle);

	centerElement(m_GameOverTitle);
	centerElement(m_GameOverSubtitle, 0.0f, m_GameOverTitle->getHeight());

	/********************
	 *     LEVEL UP     *
	 ********************/
	m_NotificationTitle = m_UIHandler->makeText("Notification Title");
	m_NotificationSubtitle = m_UIHandler->makeText("Notification Subtitle");

	setupTitleStyle(m_NotificationTitle);
	setupSubtitleStyle(m_NotificationSubtitle);
}

void HUD::loadingEnded()
{
	m_TitleScreenTitle->setVisible(true);
	m_TitleScreenSubtitle->setVisible(true);

	if (m_LoadingScreenTitle)
	{
		m_UIHandler->removeText(m_LoadingScreenTitle);
		m_LoadingScreenTitle.reset();
	}
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
	showNotification("LEVEL UP!", event.message);
}

void HUD::onDoubleScoreStarted(DoubleScoreStarted event)
{
	showNotification("DOUBLE SCORE!", "Kill them all!");
}

void HUD::onDoubleScoreOver(DoubleScoreOver event)
{
	showNotification("DOUBLE SCORE OVER", "Back to normal.");
}

void HUD::showNotification(String title, String subtitle)
{
	m_NotificationTitle->setText(title);
	m_NotificationSubtitle->setText(subtitle);

	Application::getScene()->callLater([this]() {
		centerElement(m_NotificationTitle, 0.0f, -200.0f);
		centerElement(m_NotificationSubtitle, 0.0f, -200.0f + m_NotificationTitle->getHeight());

		m_NotificationTitle->setVisible(true);
		m_NotificationSubtitle->setVisible(true);
	});

	Application::getScene()->makeTimer(1.0f)->addCallback([this](TimerTimeoutEvent) {
		m_NotificationTitle->setVisible(false);
		m_NotificationSubtitle->setVisible(false);
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

	f32 duration = std::min(event.fireCooldown * 0.45f, 0.15f);

	tween->addMethodTweener(cb, initialSize, finalSize, duration);
	tween->addMethodTweener(cb, finalSize, initialSize, duration);
}

void HUD::onGameStateChanged(GameStateChanged event)
{
	if (event.gameState == GameState::TITLE)
	{
		m_TitleScreenTitle->setVisible(true);
		m_TitleScreenSubtitle->setVisible(true);
	}
	else if (m_TitleActive)
	{
		m_TitleActive = false;

		m_UIHandler->removeText(m_TitleScreenTitle);
		m_UIHandler->removeText(m_TitleScreenSubtitle);
		m_TitleTween->stop();

		m_HPText->setVisible(true);
		m_DashesText->setVisible(true);
		m_ScoreText->setVisible(true);
		m_Crosshair->setVisible(true);
	}

	m_PauseScreenTitle->setVisible(event.gameState == GameState::PAUSE);
	m_PauseScreenSubtitle->setVisible(event.gameState == GameState::PAUSE);

	m_GameOverTitle->setVisible(event.gameState == GameState::GAME_OVER);
	m_GameOverSubtitle->setVisible(event.gameState == GameState::GAME_OVER);
}

void HUD::onScoreUpdated(ScoreUpdated event)
{
	m_ScoreText->setText(stringFormat("Score: %d", event.score));
}

void setupStatsTextStyle(Ref<UIText> text)
{
	text->setColor(0.22f, 0.22f, 0.22f);
	text->setStroke(0.5f);
	text->setVisible(false);
}

void setupTitleStyle(Ref<UIText> text)
{
	text->setStroke(0.6f);
	text->setBorder(true);
	text->setBorderColor(0.0f, 0.0f, 0.0f);
	text->setVisible(false);
}
void setupSubtitleStyle(Ref<UIText> text)
{
	text->setColor(0.33f, 0.33f, 0.33f);
	text->setBorder(true);
	text->setBorderColor(0.0f, 0.0f, 0.0f);
	text->setVisible(false);
}

} // namespace game
