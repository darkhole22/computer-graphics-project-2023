#pragma once

#include "vulture/core/Application.h"
#include "vulture/core/Input.h"

namespace game {

using namespace vulture;

class TweenTest
{
public:
	TweenTest()
	{
		m_Scene = Application::getScene();
		m_HandlerUI = m_Scene->getUIHandle();

		tweenText = m_HandlerUI->makeText("Tween: running");
		tweenText->setPosition({ 20, 100 });
		std::function<void(float)> callback = [this](float size) {
			tweenText->setSize(size);
		};
		tween = m_Scene->makeTween();
		tween->loop();
		tween->addIntervalTweener(0.5);
		auto parallel = tween->addParallelTweener();
		{
			auto sequential = parallel->addSequentialTweener();
			{
				sequential->addValueTweener(&tweenValue2, 3000ULL, 0.5);
				sequential->addValueTweener(&tweenValue2, 150ULL, 0.5);
				sequential->addCallbackTweener([this]() {
					const glm::vec3 colors[3] = {
						{1.0f, 0.0f, 0.0f},
						{0.0f, 1.0f, 0.0f},
						{0.0f, 0.0f, 1.0f}
					};
					static u64 index = 0;
					tweenText->setColor(colors[index]);
					index = (index + 1) % 3;
				});
			}
			parallel->addValueTweener(&tweenValue, { 20, 300 }, 1);
			parallel->addMethodTweener(callback, 22.0f, 40.0f, 1.0f);
		}
		tween->addIntervalTweener(0.5);
		tween->addValueTweener(&tweenValue, { 250, 250 }, 1.5f);
		tween->addValueTweener(&tweenValue, { 20, 100 }, 1.0f);
		tween->addMethodTweener(callback, 40.0f, 22.0f, 0.5f);

	}

	void update(f64 dt)
	{
		static f64 time = 0;
		time += dt;

		static bool wasKPressed = false;
		bool isKPressed = Input::isKeyPressed(GLFW_KEY_K);
		if (isKPressed && !wasKPressed)
		{
			tween->stop();
		}
		wasKPressed = isKPressed;

		tweenText->setText(stringFormat("Time %f\nValue: %llu", time, tweenValue2));
		tweenText->setPosition(tweenValue);
	}

private:
	Scene* m_Scene = nullptr;
	UIHandler* m_HandlerUI = nullptr;

	Ref<Tween> tween;
	Ref<UIText> tweenText;
	glm::vec2 tweenValue = { 10, 10 };
	u64 tweenValue2 = 15;

};

} // namespace game