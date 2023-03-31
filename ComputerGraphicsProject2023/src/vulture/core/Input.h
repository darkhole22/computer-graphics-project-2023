#pragma once

#include <utility>

#include <GLFW/glfw3.h>

#include "vulture/renderer/Window.h"
#include "vulture/core/Core.h"

namespace vulture {

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
		s_Actions[actionName] = std::move(action);
	}

	static bool isActionPressed(const std::string& actionName)
	{
		if(s_Actions.find(actionName) == s_Actions.end())
		{
			return false;
		}

		auto action = s_Actions[actionName];
		for (const auto& keyboardBinding : action.keyboardBindings)
		{
			bool actionPressed = true;
			for (auto key : keyboardBinding.keys)
			{
				if (!isKeyPressed(key)) actionPressed = false;
			}

			if (actionPressed) return true;
		}

		for (const auto& mouseBinding : action.mouseBindings)
		{
			bool actionPressed = true;
			for (auto btn : mouseBinding.buttons)
			{
				if (!isMouseButtonPressed(btn)) actionPressed = false;
			}

			if (actionPressed) return true;
		}

		for (const auto& gamepadButtonBinding : action.mouseBindings)
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

	static bool isKeyPressed(int keyCode)
	{
		if(glfwGetKey(s_Window->getHandle(), keyCode) == GLFW_PRESS)
		{
			return true;
		}

		return false;
	}

	static bool isMouseButtonPressed(int buttonCode)
	{
		if(glfwGetMouseButton(s_Window->getHandle(), buttonCode) == GLFW_PRESS)
		{
			return true;
		}

		return false;
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

private:
	Input() = default;

	inline static Window const* s_Window;
	inline static std::unordered_map<std::string, InputAction> s_Actions;

	static void initialize(const Window& window)
	{
		s_Window = &window;
	}

	friend class Application;
};

} // vulture