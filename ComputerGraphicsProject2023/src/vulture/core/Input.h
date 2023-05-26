#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

#include "vulture/renderer/Window.h"

namespace vulture {

constexpr int GAMEPAD_AXIS_POS = 1;
constexpr int GAMEPAD_AXIS_NEG = -1;

constexpr int MOUSE_AXIS_POS_X = 1;
constexpr int MOUSE_AXIS_NEG_X = -1;
constexpr int MOUSE_AXIS_POS_Y = 1;
constexpr int MOUSE_AXIS_NEG_Y = -1;

enum class MouseAxix : i32
{
	POSITIVE_X, NEGATIVE_X,
	POSITIVE_Y, NEGATIVE_Y
};

/**
 * InputStatus struct represents the current status of an input device such as keyboard, mouse, or gamepad button or gamepad axis.
 * The struct includes the following properties:
 * - isPressed (bool): true if the input is currently pressed, false otherwise
 * - isJustReleased (bool): true if the input was just released in the current frame, false otherwise
 * - strength (float): the strength of the input, which can be used for analog inputs such as joysticks. Range is -1.0f to 1.0f
 * - lastStrength (float): the strength of the input in the previous frame. This is useful for detecting changes in input strength.
 */
struct InputStatus
{
	bool isPressed;
	bool isJustReleased;

	float strength;
	float lastStrength;
};

/**
 * KeyboardBinding struct represents a binding for one or more keys on the keyboard.
 * The struct includes the following property:
 * - keys (vector<int>): a vector of key codes for the keys that are bound to this action.
 *   Each key code is a unique integer value that represents a specific key on the keyboard.
 */
struct KeyboardBinding
{
	std::vector<int> keys;
};

/**
 * MouseBinding struct represents a binding for one or more buttons on the mouse.
 * The struct includes the following property:
 * - buttons (vector<int>): a vector of button codes for the buttons that are bound to this action.
 *   Each button code is a unique integer value that represents a specific button on the mouse.
 */
struct MouseBinding
{
	std::vector<int> buttons;
};

/**
 * @brief MouseAxisBinding struct represents a binding for one or more axes on the mouse.
 * The struct includes the following property:
 * - axes (std::vector<MouseAxix>): a vector of axis codes and direction values that are bound to this action.
 */
struct MouseAxisBinding
{
	std::vector<MouseAxix> axes;
};

/**
 * GamepadButtonBinding struct represents a binding for one or more buttons on the gamepad.
 * The struct includes the following property:
 * - buttons (vector<int>): a vector of button codes for the buttons that are bound to this action.
 *   Each button code is a unique integer value that represents a specific button on the gamepad.
 */
struct GamepadButtonBinding
{
	std::vector<int> buttons;
};

/**
 * GamepadAxisBinding struct represents a binding for one or more axes on the gamepad.
 * The struct includes the following property:
 * - axes (vector<pair<int, int>>): a vector of pairs of axis codes and direction values that are bound to this action.
 *   Each pair consists of two integer values: the first value is the axis code, which represents a specific axis on the gamepad,
 *   and the second value is the direction, which can be either GAMEPAD_AXIS_POS or GAMEPAD_AXIS_NEG and represents the  or negative direction of the axis.
 */
struct GamepadAxisBinding
{
	std::vector<std::pair<int, int>> axes;
};


/**
 * InputAction struct represents an action that can be triggered by different input bindings on different input devices.
 * The struct includes the following properties:
 * - keyboardBindings (vector<KeyboardBinding>): a vector of keyboard bindings for the action.
 * - mouseBindings (vector<MouseBinding>): a vector of mouse bindings for the action.
 * - mouseAxisBindings (vector<MouseAxisBinding>): a vector of mouse axis bindings for the action.
 * - gamepadButtonBindings (vector<GamepadButtonBinding>): a vector of gamepad button bindings for the action.
 * - gamepadAxisBindings (vector<GamepadAxisBinding>): a vector of gamepad axis bindings for the action.
 */
struct InputAction
{
	std::vector<KeyboardBinding> keyboardBindings;
	std::vector<MouseBinding> mouseBindings;
	std::vector<MouseAxisBinding> mouseAxisBindings;
	std::vector<GamepadButtonBinding> gamepadButtonBindings;
	std::vector<GamepadAxisBinding> gamepadAxisBindings;
};

/*
 * The `Input` class provides static methods to query the state of user input.
 * It supports keyboard, mouse, and gamepad input, and provides functionality for mapping
 * inputs to named "actions" which can then be queried in a device-agnostic manner.
 */
class Input
{
public:
	/**
	 * The default constructor is deleted to prevent creating instances of this class.
	 */
	Input() = delete;

	static void setAction(const String& actionName, const InputAction& action);

	/**
	 * @brief Checks whether an input action with the given name is currently being pressed.
	 *
	 * @param actionName The name of the action to check.
	 * @return True if the action is being pressed, false otherwise.
	 */
	inline static bool isActionPressed(const String& actionName) { return getActionStrength(actionName) > 0.0f; }

	/**
	 * @brief Checks whether an input action with the given name is currently being pressed and was not pressed the last frame.
	 *
	 * @param actionName The name of the action to check.
	 * @return True if the action is being just pressed, false otherwise.
	 */
	static bool isActionJustPressed(const String& actionName);

	/**
	 * @brief Checks whether an input action with the given name has been released in the current frame.
	 *
	 * @param actionName The name of the action to check.
	 * @return True if the action has been released, false otherwise.
	 */
	static bool isActionReleased(const String& actionName);

	/**
	 * @brief Returns the strength of an input action with the given name.
	 *
	 * @param actionName The name of the action to check.
	 * @return The strength of the action as a float value between 0 and 1.
	 */
	static float getActionStrength(const String& actionName);

	/**
	 * @brief Returns the value of a virtual input axis.
	 *
	 * @param negativeAction The name of the negative input action.
	 * @param positiveAction The name of the positive input action.
	 * @return The value of the input axis as a float value between -1 and 1.
	 */
	inline static float getAxis(const String& negativeAction, const String& positiveAction)
	{
		return getActionStrength(positiveAction) - getActionStrength(negativeAction);
	}

	/**
	 * @brief Returns a 2D vector based on the input actions assigned to the given input bindings.
	 *
	 * @param negativeX The name of the negative X-axis input action.
	 * @param positiveX The name of the positive X-axis input action.
	 * @param negativeY The name of the negative Y-axis input action.
	 * @param positiveY The name of the positive Y-axis input action.
	 * @return The input vector as a glm::vec2.
	 */
	static glm::vec2 getVector(const String& negativeX, const String& positiveX, const String& negativeY, const String& positiveY);

	static glm::vec2 getMouseVector();

	/**
	 * @brief Checks whether a keyboard key with the given key code is currently being pressed.
	 *
	 * @param keyCode The key code of the key to check.
	 * @return True if the key is being pressed, false otherwise.
	 */
	static bool isKeyPressed(int keyCode);

	/**
	 * @brief Checks whether a mouse button with the given button code is currently being pressed.
	 *
	 * @param buttonCode The button code of the mouse button to check.
	 * @return True if the mouse button is being pressed, false otherwise.
	 */
	static bool isMouseButtonPressed(int buttonCode);

	/**
	 * @brief Checks whether a gamepad button with the given button code is currently being pressed.
	 *
	 * @param buttonCode The button code of the gamepad button to check.
	 * @return True if the gamepad button is being pressed, false otherwise.
	 */
	static bool isGamepadButtonPressed(int buttonCode);

	/**
	 * @brief Returns the strength of a gamepad axis.
	 *
	 * @param axis The axis code of the gamepad axis to check.
	 * @return The strength of the axis as a float value between -1 and 1.
	 */
	static float getGamepadAxis(int axis);

private:
	inline static f64 s_MouseXPosition = 0;
	inline static f64 s_MouseYPosition = 0;
	inline static f64 s_MouseXOldPosition = 0;
	inline static f64 s_MouseYOldPosition = 0;
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
		getCursorPositionStatus();
	}

	static void onGlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void onGlfwMouseButton(GLFWwindow* window, int button, int action, int mods);

	static void getCursorPositionStatus();

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

	static bool detectMouseAxisActionReleased(const std::vector<MouseAxix>& bindings);

	// detectActionPressed checks if all the given bindings are pressed at the same time.
	static bool detectActionPressed(const std::vector<int>& bindings, int baseIndex);

	static bool detectActionJustPressed(const std::vector<int>& bindings, int baseIndex);

	// This is specific for gamepad axis, it is needed to discriminate positive and negative axis values for different actions.
	static float getActionAxisStrength(const std::vector<std::pair<int, int>>& bindings);

	static float getActionMouseAxisStrength(const std::vector<MouseAxix>& bindings);

	// resetReleased resets the `isJustReleased` status of every registered binding.
	static void resetReleased();

	static void cleanup();

	friend class Application;
};

} // vulture