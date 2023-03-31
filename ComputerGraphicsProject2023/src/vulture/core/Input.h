#pragma once

#include <utility>

#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>

#include "vulture/renderer/Window.h"
#include "vulture/core/Core.h"

namespace vulture {

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
				s_KeyStatuses.try_emplace(key, new InputStatus{});
			}
		}

		s_Actions[actionName] = std::move(action);
	}

	static bool isActionPressed(const std::string& actionName)
	{
		auto it = s_Actions.find(actionName);
		if(it == s_Actions.end()) return false;

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

	static bool isKeyPressed(int keyCode)
	{
		auto it = s_KeyStatuses.find(keyCode);
		if (it == s_KeyStatuses.end())
		{
			return glfwGetKey(s_Window->getHandle(), keyCode) == GLFW_PRESS;
		}

		return it->second->isPressed;
	}

	static bool isMouseButtonPressed(int buttonCode)
	{
		auto it = s_MouseButtonsStatuses.find(buttonCode);
		if (it == s_MouseButtonsStatuses.end())
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

	inline static std::unordered_map<int, InputStatus*> s_KeyStatuses;
	inline static std::unordered_map<int, InputStatus*> s_MouseButtonsStatuses;

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
		auto it = s_KeyStatuses.find(key);
		if (it == s_KeyStatuses.end()) return;

		it->second->isPressed = action != GLFW_RELEASE;
		it->second->isJustReleased = action == GLFW_RELEASE;
	}

	static void onGlfwMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		auto it = s_MouseButtonsStatuses.find(button);
		if (it == s_MouseButtonsStatuses.end()) return;

		it->second->isPressed = action != GLFW_RELEASE;
		it->second->isJustReleased = action == GLFW_RELEASE;
	}

	static void resetReleased()
	{
		for (auto keyStatus: s_KeyStatuses)
		{
			keyStatus.second->isJustReleased = false;
		}

		for (auto btnStatus : s_KeyStatuses)
		{
			btnStatus.second->isJustReleased = false;
		}
	}

	friend class Application;
};

} // vulture