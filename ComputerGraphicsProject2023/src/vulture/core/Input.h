#pragma once

#include <utility>

#include <GLFW/glfw3.h>
#include <unordered_map>

#include "vulture/renderer/Window.h"
#include "vulture/core/Core.h"

namespace vulture {

#define KEY_IDX(K) K
#define MOUSE_BTN_IDX(BTN) (BTN + GLFW_KEY_LAST + 1)
#define GAMEPAD_BTN_IDX(BTN) (BTN + MOUSE_BTN_IDX(GLFW_MOUSE_BUTTON_LAST) + 1)
#define GAMEPAD_AXIS_IDX(AXIS) (AXIS + GAMEPAD_BTN_IDX(GLFW_GAMEPAD_BUTTON_LAST) + 1)
#define GAMEPAD_AXIS_POS true
#define GAMEPAD_AXIS_NEG false

struct InputStatus
{
	bool isPressed;
	bool isJustReleased;
	float strength;
};

struct KeyboardBinding
{
	std::vector<int> keys;
};

struct MouseBinding
{
	std::vector<int> buttons;
};

struct GamepadButtonBinding
{
	std::vector<int> buttons;
};

struct GamepadAxisBinding
{
	std::vector<std::pair<int, bool>> axes;
};

struct InputAction
{
	std::vector<KeyboardBinding> keyboardBindings;
	std::vector<MouseBinding> mouseBindings;
	std::vector<GamepadButtonBinding> gamepadButtonBindings;
	std::vector<GamepadAxisBinding> gamepadAxisBindings;
};

class Input
{
public:
	inline static void setAction(const std::string& actionName, InputAction action)
	{
		for (const auto& keyboardBinding: action.keyboardBindings)
		{
			for (auto key: keyboardBinding.keys)
			{
				s_InputStatuses.try_emplace(KEY_IDX(key), new InputStatus{});
			}
		}

		for (const auto& mouseButtonBinding: action.mouseBindings)
		{
			for (auto btn: mouseButtonBinding.buttons)
			{
				s_InputStatuses.try_emplace(MOUSE_BTN_IDX(btn), new InputStatus{});
			}
		}

		for (const auto& gamepadButtonBinding: action.gamepadButtonBindings)
		{
			for (auto btn: gamepadButtonBinding.buttons)
			{
				s_InputStatuses.try_emplace(GAMEPAD_BTN_IDX(btn), new InputStatus{});
			}
		}

		for (const auto& gamepadAxisBinding: action.gamepadAxisBindings)
		{
			for (auto axis: gamepadAxisBinding.axes)
			{
				s_InputStatuses.try_emplace(GAMEPAD_AXIS_IDX(axis.first), new InputStatus{});
			}
		}

		s_Actions[actionName] = std::move(action);
	}

	inline static bool isActionPressed(const std::string& actionName)
	{
		return getActionStrength(actionName) > 0.0f;
	};

	inline static bool isActionReleased(const std::string& actionName)
	{
		auto it = s_Actions.find(actionName);
		if(it == s_Actions.end()) return false;

		auto action = it->second;
		bool mayRelease = false;

		for (const auto& keyboardBinding : action.keyboardBindings)
		{
			int res = detectActionReleased(keyboardBinding.keys, KEY_IDX(0));
			if (res == -1) return false;
			if (res == 1) mayRelease = true;
		}

		for (const auto& mouseBinding : action.mouseBindings)
		{
			int res = detectActionReleased(mouseBinding.buttons, MOUSE_BTN_IDX(0));
			if (res == -1) return false;
			if (res == 1) mayRelease = true;
		}

		for (const auto& gamepadButtonBinding : action.gamepadButtonBindings)
		{
			int res = detectActionReleased(gamepadButtonBinding.buttons, GAMEPAD_BTN_IDX(0));
			if (res == -1) return false;
			if (res == 1) mayRelease = true;
		}

		return mayRelease;
	};

	inline static float getActionStrength(const std::string& actionName)
	{
		auto it = s_Actions.find(actionName);
		if(it == s_Actions.end()) return false;

		auto action = it->second;

		for (const auto& keyboardBinding : action.keyboardBindings)
		{
			if (detectActionPressed(keyboardBinding.keys, KEY_IDX(0))) return 1.0f;
		}

		for (const auto& mouseBinding : action.mouseBindings)
		{
			if (detectActionPressed(mouseBinding.buttons, MOUSE_BTN_IDX(0))) return 1.0f;
		}

		for (const auto& gamepadButtonBinding : action.gamepadButtonBindings)
		{
			if (detectActionPressed(gamepadButtonBinding.buttons, GAMEPAD_BTN_IDX(0))) return 1.0f;
		}

		float maxStrength = 0.0f;
		for (const auto& gamepadAxisBinding: action.gamepadAxisBindings)
		{
			float str = getActionAxisStrength(gamepadAxisBinding.axes);
			if (str > maxStrength) maxStrength = str;
		}

		return maxStrength;
	}

	inline static float getAxis(const std::string& negativeAction, const std::string& positiveAction) {
		return getActionStrength(positiveAction) - getActionStrength(negativeAction);
	}

	inline static bool isKeyPressed(int keyCode)
	{
		auto it = s_InputStatuses.find(KEY_IDX(keyCode));
		if (it == s_InputStatuses.end())
		{
			return glfwGetKey(s_Window->getHandle(), keyCode) == GLFW_PRESS;
		}

		return it->second->isPressed;
	}

	inline static bool isMouseButtonPressed(int buttonCode)
	{
		auto it = s_InputStatuses.find(MOUSE_BTN_IDX(buttonCode));
		if (it == s_InputStatuses.end())
		{
			return glfwGetMouseButton(s_Window->getHandle(), buttonCode) == GLFW_PRESS;
		}

		return it->second->isPressed;
	}

	inline static bool isGamepadButtonPressed(int buttonCode)
	{
		auto it = s_InputStatuses.find(GAMEPAD_BTN_IDX(buttonCode));

		if (it == s_InputStatuses.end())
		{
			GLFWgamepadstate state;
			if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
			{
				return state.buttons[buttonCode] == GLFW_PRESS;
			}

			return false;
		}

		return it->second->isPressed;
	}

	inline static float getGamepadAxis(int axis)
	{
		auto it = s_InputStatuses.find(GAMEPAD_AXIS_IDX(axis));
		if (it == s_InputStatuses.end())
		{
			GLFWgamepadstate state;
			if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
			{
				if (std::abs(state.axes[axis]) >= GAMEPAD_AXIS_DEADZONE) return state.axes[axis];
			}

			return 0.0f;
		}

		return it->second->strength;
	}

private:
	Input() = default;

	inline static Window const* s_Window;
	inline static std::unordered_map<std::string, InputAction> s_Actions;

	inline static std::unordered_map<int, InputStatus*> s_InputStatuses;

	inline static const float GAMEPAD_AXIS_DEADZONE = 0.25f;

	inline static void initialize(const Window& window)
	{
		s_Window = &window;

		glfwSetKeyCallback(s_Window->getHandle(), onGlfwKey);
		glfwSetMouseButtonCallback(s_Window->getHandle(), onGlfwMouseButton);
	}

	// This should be called once per frame, before polling
	// events from the window.
	inline static void cleanup()
	{
		resetReleased();
		getGamepadInputStatus();
	}

	inline static void onGlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto it = s_InputStatuses.find(KEY_IDX(key));
		if (it == s_InputStatuses.end()) return;

		it->second->isPressed = action != GLFW_RELEASE;
		it->second->isJustReleased = action == GLFW_RELEASE;
	}

	inline static void onGlfwMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		auto it = s_InputStatuses.find(MOUSE_BTN_IDX(button));
		if (it == s_InputStatuses.end()) return;

		it->second->isPressed = action != GLFW_RELEASE;
		it->second->isJustReleased = action == GLFW_RELEASE;
	}

	// getGamepadInputStatus collect the current status of all registered gamepad buttons bindings.
	// This is necessary since GLFW does not provide any callback-based access to Gamepad events, so
	// we need to manually collect it each frame.
	inline static void getGamepadInputStatus() {
		GLFWgamepadstate state;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
		{
			for (int btn = 0; btn <= GLFW_GAMEPAD_BUTTON_LAST; btn++)
			{
				auto it = s_InputStatuses.find(GAMEPAD_BTN_IDX(btn));
				if(it == s_InputStatuses.end()) continue;

				it->second->isJustReleased = (it->second->isPressed && state.buttons[btn] == GLFW_RELEASE);
				it->second->isPressed = (state.buttons[btn] == GLFW_PRESS);
				it->second->strength = it->second->isPressed ? 1.0f : 0.0f;
			}

			for (int axis = 0; axis <= GLFW_GAMEPAD_AXIS_LAST; axis++)
			{
				auto it = s_InputStatuses.find(GAMEPAD_AXIS_IDX(axis));
				if (it == s_InputStatuses.end()) continue;

				float raw_str = state.axes[axis];

				it->second->isJustReleased = std::abs(raw_str) < GAMEPAD_AXIS_DEADZONE && it->second->isPressed;
				it->second->isPressed = std::abs(raw_str) >= GAMEPAD_AXIS_DEADZONE;
				it->second->strength = it->second->isPressed ? raw_str : 0.0f;
			}
		}
	}

	// detectActionReleased checks if an action bound to the given input binding could be released in the current frame.
	// It returns 1 if, according to the given bindings, it is released.
	// It returns 0 if, according to the given bindings, they're not active, but they weren't released this frame.
	// It returns -1 if, according to the given bindings, the action is active.
	inline static int detectActionReleased(const std::vector<int>& bindings, int baseIndex)
	{
		bool oneRelease = false, allPressed = true, noDead = true;

		for (auto binding : bindings)
		{
			auto inputStatus = s_InputStatuses[baseIndex + binding];
			if (!inputStatus->isPressed) allPressed = false;
			if (!inputStatus->isPressed && !inputStatus->isJustReleased) noDead = false;
			if (inputStatus->isJustReleased) oneRelease = true;
		}

		if (allPressed) return -1;
		if (oneRelease && noDead) return 1;
		return 0;
	}

	// detectActionPressed checks if all the given bindings are pressed at the same time.
	inline static bool detectActionPressed(const std::vector<int>& bindings, int baseIndex)
	{
		for (auto binding : bindings)
		{
			if (!s_InputStatuses[baseIndex + binding]->isPressed) return false;
		}

		return true;
	}

	// This is specific for gamepad axis, it is needed to discriminate positive and negative axis values for different actions.
	inline static float getActionAxisStrength(const std::vector<std::pair<int, bool>>& bindings)
	{
		float minStrength = 1.0f;

		for (auto binding : bindings)
		{
			auto status = s_InputStatuses[GAMEPAD_AXIS_IDX(binding.first)];
			if (!status->isPressed ||
				status->strength < 0 && binding.second ||
				status->strength > 0 && !binding.second) return 0.0f;

			if (std::abs(status->strength) < minStrength) minStrength = std::abs(status->strength);
		}

		return minStrength;
	}

	// resetReleased resets the `isJustReleased` status of every registered binding.
	inline static void resetReleased()
	{
		for (auto inputStatus: s_InputStatuses)
		{
			inputStatus.second->isJustReleased = false;
		}
	}

	friend class Application;
};

} // vulture