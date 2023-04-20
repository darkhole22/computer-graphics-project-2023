#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

#include "vulture/renderer/Window.h"
#include "vulture/core/Core.h"

namespace vulture {

constexpr int GAMEPAD_AXIS_POS = 1;
constexpr int GAMEPAD_AXIS_NEG = -1;

struct InputStatus
{
	bool isPressed;
	bool isJustReleased;

	float strength;
	float lastStrength;
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
	std::vector<std::pair<int, int>> axes;
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
	Input() = delete;

	static void setAction(const String& actionName, InputAction action);

	inline static bool isActionPressed(const String& actionName) { return getActionStrength(actionName) > 0.0f; }

	static bool isActionReleased(const String& actionName);

	static float getActionStrength(const String& actionName);

	inline static float getAxis(const String& negativeAction, const String& positiveAction)
	{
		return getActionStrength(positiveAction) - getActionStrength(negativeAction);
	}

	static bool isKeyPressed(int keyCode);

	static bool isMouseButtonPressed(int buttonCode);

	static bool isGamepadButtonPressed(int buttonCode);

	static float getGamepadAxis(int axis);

private:
	inline static Window const* s_Window;
	inline static std::unordered_map<String, InputAction> s_Actions;
	inline static std::unordered_map<int, InputStatus*> s_InputStatuses;
	inline static constexpr float GAMEPAD_AXIS_DEADZONE = 0.25f;

	inline static void initialize(const Window& window)
	{
		s_Window = &window;

		glfwSetKeyCallback(s_Window->getHandle(), onGlfwKey);
		glfwSetMouseButtonCallback(s_Window->getHandle(), onGlfwMouseButton);
	}

	// This should be called once per frame, before polling
	// events from the window.
	inline static void reset()
	{
		resetReleased();
		getGamepadInputStatus();
	}

	static void onGlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void onGlfwMouseButton(GLFWwindow* window, int button, int action, int mods);

	// getGamepadInputStatus collect the current status of all registered gamepad buttons bindings.
	// This is necessary since GLFW does not provide any callback-based access to Gamepad events, so
	// we need to manually collect it each frame.
	static void getGamepadInputStatus();

	// detectActionReleased checks if an action bound to the given input binding could be released in the current frame.
	// It returns 1 if, according to the given bindings, it is released.
	// It returns 0 if, according to the given bindings, they're not active, but they weren't released this frame.
	// It returns -1 if, according to the given bindings, the action is active.
	static int detectActionReleased(const std::vector<int>& bindings, int baseIndex);

	// This detects when an action performed via gamepad axes is released.
	// Be careful: this could also detect "false positives" (which, however, are valid releases)
	// caused by the gamepads analog sticks quickly bouncing from one side to the other when quickly released.
	static bool detectGamepadAxisActionReleased(const std::vector<std::pair<int, int>>& bindings);

	// detectActionPressed checks if all the given bindings are pressed at the same time.
	static bool detectActionPressed(const std::vector<int>& bindings, int baseIndex);

	// This is specific for gamepad axis, it is needed to discriminate positive and negative axis values for different actions.
	static float getActionAxisStrength(const std::vector<std::pair<int, int>>& bindings);

	// resetReleased resets the `isJustReleased` status of every registered binding.
	static void resetReleased();

	static void cleanup();

	friend class Application;
};

} // vulture