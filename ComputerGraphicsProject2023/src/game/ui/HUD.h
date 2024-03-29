#pragma once

#include "game/EventBus.h"
#include "vulture/core/Application.h"

namespace game {

using namespace vulture;

class HUD
{
public:
	HUD();

	void loadingEnded();
private:
	UIHandler* m_UIHandler;
	Window* m_Window;

	Ref<UIText> m_LoadingScreenTitle;

	Ref<UIText> m_HPText;
	Ref<UIText> m_DashesText;
	Ref<UIText> m_ScoreText;

	Ref<UIImage> m_Crosshair;

	Ref<UIText> m_TitleScreenTitle;
	Ref<UIText> m_TitleScreenSubtitle;

	Ref<UIText> m_PauseScreenTitle;
	Ref<UIText> m_PauseScreenSubtitle;

	Ref<UIText> m_GameOverTitle;
	Ref<UIText> m_GameOverSubtitle;

	Ref<UIText> m_NotificationTitle;
	Ref<UIText> m_NotificationSubtitle;

	bool m_TitleActive = true;
	Ref<Tween> m_TitleTween;

	void onHealthUpdated(HealthUpdated event);
	void onDashesUpdated(DashesUpdated event);
	void onBulletShot(BulletShot event);
	void onLevelUp(LevelUp event);
	void onDoubleScoreStarted(DoubleScoreStarted event);
	void onDoubleScoreOver(DoubleScoreOver event);

	void showNotification(String title, String subtitle);

	void onGameStateChanged(GameStateChanged event);
	void onScoreUpdated(ScoreUpdated event);

	template <class UIElement>
	void centerElement(Ref<UIElement> element, f32 xOffset = 0.0f, f32 yOffset = 0.0f)
	{
		element->setPosition((m_Window->getWidth() - element->getWidth()) / 2.0f + xOffset,
						  (m_Window->getHeight() - element->getHeight()) / 2.0f + yOffset);
	}
};

}