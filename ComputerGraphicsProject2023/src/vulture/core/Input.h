#pragma once

#include <utility>

#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>

#include "vulture/renderer/Window.h"
#include "vulture/core/Core.h"

namespace vulture {

#define KEY_IDX(K) K
#define MOUSE_BTN_IDX(BTN) (BTN + GLFW_KEY_LAST + 1)
#define GAMEPAD_BTN_IDX(BTN) (BTN + GLFW_MOUSE_BUTTON_LAST + 1)

struct InputStatus
{
	bool isPressed;
	bool isJustReleased;
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

struct InputAction
{
	std::vector<KeyboardBinding> keyboardBindings;
	std::vector<MouseBinding> mouseBindings;
	std::vector<GamepadButtonBinding> gamepadButtonBindings;
};

class Input
{
public:
	static void setAction(const std::string& actionName, InputAction action)
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

		s_Actions[actionName] = std::move(action);
	}

	static bool isActionPressed(const std::string& actionName)
	{
		auto it = s_Actions.find(actionName);
		if(it == s_Actions.end()) return false;

		auto action = it->second;

		for (const auto& keyboardBinding : action.keyboardBindings)
		{
			bool actionPressed = true;
			for (auto key : keyboardBinding.keys)
			{
				if (!s_InputStatuses[KEY_IDX(key)]->isPressed) actionPressed = false;
			}
			if (actionPressed) return true;
		}

		for (const auto& mouseBinding : action.mouseBindings)
		{
			bool actionPressed = true;
			for (auto btn : mouseBinding.buttons)
			{
				if (!s_InputStatuses[MOUSE_BTN_IDX(btn)]->isPressed) actionPressed = false;
			}

			if (actionPressed) return true;
		}

		for (const auto& gamepadButtonBinding : action.gamepadButtonBindings)
		{
			bool actionPressed = true;
			for (auto btn : gamepadButtonBinding.buttons)
			{
				if (!isGamepadButtonPressed(btn)) actionPressed = false;
			}

			if (actionPressed) return true;
		}

		return false;
	};

	static bool isActionReleased(const std::string& actionName)
	{
		auto it = s_Actions.find(actionName);
		if(it == s_Actions.end()) return false;

		auto action = it->second;
		bool anyRelease = false;

		for (const auto& keyboardBinding : action.keyboardBindings)
		{
			bool allPressed = true;
			for (auto key : keyboardBinding.keys)
			{
				auto keyStatus = s_InputStatuses[KEY_IDX(key)];
				if (!keyStatus->isPressed) allPressed = false;
				if (keyStatus->isJustReleased) anyRelease = true;
			}

			if (allPressed) return false;
		}

		for (const auto& mouseBinding : action.mouseBindings)
		{
			bool allPressed = true;
			for (auto btn : mouseBinding.buttons)
			{
				auto btnStatus = s_InputStatuses[MOUSE_BTN_IDX(btn)];
				if (!btnStatus->isPressed) allPressed = false;
				if (btnStatus->isJustReleased) anyRelease = true;
			}

			if (allPressed) return false;
		}

		for (const auto& gamepadButtonBinding : action.gamepadButtonBindings)
		{
			bool allPressed = true;
			for (auto btn : gamepadButtonBinding.buttons)
			{
				if (!isGamepadButtonPressed(btn)) allPressed = false;
			}

			if (allPressed) return false;
		}

		return anyRelease;
	};


	static bool isKeyPressed(int keyCode)
	{
		auto it = s_InputStatuses.find(KEY_IDX(keyCode));
		if (it == s_InputStatuses.end())
		{
			return glfwGetKey(s_Window->getHandle(), keyCode) == GLFW_PRESS;
		}

		return it->second->isPressed;
	}

	static bool isMouseButtonPressed(int buttonCode)
	{
		auto it = s_InputStatuses.find(MOUSE_BTN_IDX(buttonCode));
		if (it == s_InputStatuses.end())
		{
			return glfwGetMouseButton(s_Window->getHandle(), buttonCode) == GLFW_PRESS;
		}

		return it->second->isPressed;
	}

	static bool isGamepadButtonPressed(int buttonCode)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
		{
			if (state.buttons[buttonCode])
			{
				return true;
			}
		}

		return false;
	}

	static float getGamepadAxis(int axis)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
		{
			return state.axes[axis];
		}

		return 0.0f;
	}

private:
	Input() = default;

	inline static Window const* s_Window;
	inline static std::unordered_map<std::string, InputAction> s_Actions;

	inline static std::unordered_map<int, InputStatus*> s_InputStatuses;

	static void initialize(const Window& window)
	{
		s_Window = &window;

		glfwSetKeyCallback(s_Window->getHandle(), onGlfwKey);
		glfwSetMouseButtonCallback(s_Window->getHandle(), onGlfwMouseButton);
	}

	// This should be called once per frame, before polling
	// events from the window.
	static void cleanup()
	{
		resetReleased();
	}

	static void onGlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto it = s_InputStatuses.find(KEY_IDX(key));
		if (it == s_InputStatuses.end()) return;

		it->second->isPressed = action != GLFW_RELEASE;
		it->second->isJustReleased = action == GLFW_RELEASE;
	}

	static void onGlfwMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		auto it = s_InputStatuses.find(MOUSE_BTN_IDX(button));
		if (it == s_InputStatuses.end()) return;

		it->second->isPressed = action != GLFW_RELEASE;
		it->second->isJustReleased = action == GLFW_RELEASE;
	}

	static void resetReleased()
	{
		for (auto inputStatus: s_InputStatuses)
		{
			inputStatus.second->isJustReleased = false;
		}
	}

	friend class Application;
};

} // vulture