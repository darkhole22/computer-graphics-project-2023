#pragma once

#include "vulture/scene/ui/UIHandler.h"
#include "vulture/core/Application.h"
#include "vulture/core/Input.h"

using namespace vulture;

namespace game {

class UI
{
private:
	UIHandler *handlerUI = nullptr;

	Ref<UIText> text;
	Ref<UIText> text2;
public:
	UI()
	{
		handlerUI = Application::getScene()->getUIHandle();

		text = handlerUI->makeText("FPS");
		text2 = handlerUI->makeText("Frame Time");
		text2->setPosition({20, 50});

		text->setVisible(false);
		text2->setVisible(false);
	}

	void update(float dt)
	{
		// Press F3 to toggle info
		static bool wasF3Pressed = false;
		bool isF3Pressed = Input::isKeyPressed(GLFW_KEY_F3);
		if (isF3Pressed && !wasF3Pressed) {
			text->setVisible(!text->isVisible());
			text2->setVisible(!text2->isVisible());
		}
		wasF3Pressed = isF3Pressed;

		static float fps = 0.0f;
		static float delta = 0;

		static const float WRITE_FPS_TIMEOUT = 0.5; // seconds
		static const float FPS_AVG_WEIGHT = 0.1f;    // 0 <= x <= 1

		delta += dt;
		fps = fps * (1.0f - FPS_AVG_WEIGHT) + (1.0f / dt) * FPS_AVG_WEIGHT;

		if (delta > WRITE_FPS_TIMEOUT) {
			text->setText(stringFormat("FPS: %.0f", fps));
			text2->setText(stringFormat("Frame time: %.4fms", dt * 1000));

			delta -= WRITE_FPS_TIMEOUT;
		}
	}

};

} // namespace game
