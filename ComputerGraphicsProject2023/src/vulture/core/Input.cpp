#include "vulture/core/Input.h"

namespace vulture {

#define KEY_IDX(K) K
#define MOUSE_BTN_IDX(BTN) (BTN + GLFW_KEY_LAST + 1)
#define GAMEPAD_BTN_IDX(BTN) (BTN + MOUSE_BTN_IDX(GLFW_MOUSE_BUTTON_LAST) + 1)
#define GAMEPAD_AXIS_IDX(AXIS) (AXIS + GAMEPAD_BTN_IDX(GLFW_GAMEPAD_BUTTON_LAST) + 1)

void Input::setAction(const String& actionName, const InputAction& action)
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
		for (auto& axis: gamepadAxisBinding.axes)
		{
			s_InputStatuses.try_emplace(GAMEPAD_AXIS_IDX(axis.first), new InputStatus{});
		}
	}

	s_Actions[actionName] = action;
}

bool Input::isActionJustPressed(const String& actionName)
{
	auto it = s_Actions.find(actionName);
	if (it == s_Actions.end()) return false;

	auto& action = it->second;

	for (const auto& keyboardBinding : action.keyboardBindings)
	{
		if (detectActionJustPressed(keyboardBinding.keys, KEY_IDX(0))) return true;
	}

	for (const auto& mouseBinding : action.mouseBindings)
	{
		if (detectActionJustPressed(mouseBinding.buttons, MOUSE_BTN_IDX(0))) return true;
	}


	for (const auto& gamepadButtonBinding : action.gamepadButtonBindings)
	{
		if (detectActionJustPressed(gamepadButtonBinding.buttons, GAMEPAD_BTN_IDX(0))) return true;
	}

	for (const auto& gamepadAxisBinding: action.gamepadAxisBindings)
	{
		bool isPressed = true;
		bool wasPressed = true;
		for (auto axes: gamepadAxisBinding.axes)
		{
			if (s_InputStatuses[GAMEPAD_AXIS_IDX(axes.first)]->strength * axes.second <= 0.0f) isPressed = false;
			if(s_InputStatuses[GAMEPAD_AXIS_IDX(axes.first)]->lastStrength == 0.0f) wasPressed = false;
		}

		if (isPressed && !wasPressed) return true;
	}

	return false;
}

bool Input::detectActionJustPressed(const std::vector<int> &bindings, int baseIndex)
{
	bool wasPressed = true;
	for (auto binding: bindings)
	{
		if (s_InputStatuses[baseIndex + binding]->strength == 0.0f) return false;
		if(s_InputStatuses[baseIndex + binding]->lastStrength == 0.0f) wasPressed = false;
	}

	return !wasPressed;
}

bool Input::isActionReleased(const String& actionName)
{
	auto it = s_Actions.find(actionName);
	if(it == s_Actions.end()) return false;

	auto& action = it->second;
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

	for (const auto& gamepadAxisBinding : action.gamepadAxisBindings)
	{
		return detectGamepadAxisActionReleased(gamepadAxisBinding.axes);
	}

	return mayRelease;
}

float Input::getActionStrength(const String& actionName)
{
	auto it = s_Actions.find(actionName);
	if(it == s_Actions.end()) {
		return false;
	}

	auto& action = it->second;

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

glm::vec2 Input::getVector(const String& negativeX, const String& positiveX, const String& negativeY, const String& positiveY)
{
	glm::vec2 v(getAxis(negativeX, positiveX), getAxis(negativeY, positiveY));
	if (glm::length(v) > 1.0f) {
		v = glm::normalize(v);
	}

	return v;
}

bool Input::isKeyPressed(int keyCode)
{
	auto it = s_InputStatuses.find(KEY_IDX(keyCode));
	if (it == s_InputStatuses.end())
	{
		return glfwGetKey(s_Window->getHandle(), keyCode) == GLFW_PRESS;
	}

	return it->second->isPressed;
}

bool Input::isMouseButtonPressed(int buttonCode)
{
	auto it = s_InputStatuses.find(MOUSE_BTN_IDX(buttonCode));
	if (it == s_InputStatuses.end())
	{
		return glfwGetMouseButton(s_Window->getHandle(), buttonCode) == GLFW_PRESS;
	}

	return it->second->isPressed;
}

bool Input::isGamepadButtonPressed(int buttonCode)
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

float Input::getGamepadAxis(int axis)
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

void Input::onGlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto it = s_InputStatuses.find(KEY_IDX(key));
	if (it == s_InputStatuses.end()) return;

	it->second->strength = action != GLFW_RELEASE ? 1.0f : 0.0f;
	it->second->isPressed = action != GLFW_RELEASE;
	it->second->isJustReleased = action == GLFW_RELEASE;
}

void Input::onGlfwMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	auto it = s_InputStatuses.find(MOUSE_BTN_IDX(button));
	if (it == s_InputStatuses.end()) return;

	it->second->strength = action != GLFW_RELEASE ? 1.0f : 0.0f;
	it->second->isPressed = action != GLFW_RELEASE;
	it->second->isJustReleased = action == GLFW_RELEASE;
}

void Input::getGamepadInputStatus() {
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

			it->second->lastStrength = it->second->strength;
			it->second->strength = it->second->isPressed ? raw_str : 0.0f;
		}
	}
}

int Input::detectActionReleased(const std::vector<int>& bindings, int baseIndex)
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

bool Input::detectGamepadAxisActionReleased(const std::vector<std::pair<int, int>>& bindings)
{
	float strength = getActionAxisStrength(bindings);
	if (strength > 0) return false;

	for (auto& binding: bindings)
	{
		auto status = s_InputStatuses[GAMEPAD_AXIS_IDX(binding.first)];
		if (!status->isJustReleased || status->lastStrength * binding.second <= 0) return false;
	}

	return true;
}

// detectActionPressed checks if all the given bindings are pressed at the same time.
bool Input::detectActionPressed(const std::vector<int>& bindings, int baseIndex)
{
	for (auto binding : bindings)
	{
		if (!s_InputStatuses[baseIndex + binding]->isPressed) return false;
	}

	return true;
}

// This is specific for gamepad axis, it is needed to discriminate positive and negative axis values for different actions.
float Input::getActionAxisStrength(const std::vector<std::pair<int, int>>& bindings)
{
	float minStrength = 1.0f;

	for (auto& binding : bindings)
	{
		auto status = s_InputStatuses[GAMEPAD_AXIS_IDX(binding.first)];
		if (!status->isPressed ||
			status->strength * binding.second <= 0) return 0.0f;

		if (std::abs(status->strength) < minStrength) minStrength = std::abs(status->strength);
	}

	return minStrength;
}

void Input::resetReleased()
{
	for (auto& inputStatus : s_InputStatuses)
	{
		inputStatus.second->isJustReleased = false;

		inputStatus.second->lastStrength = inputStatus.second->strength;
	}
}

void Input::cleanup()
{
	s_Actions.clear();

	for (auto& status : s_InputStatuses)
	{
		free(status.second);
	}

	s_InputStatuses.clear();
}

} // vulture