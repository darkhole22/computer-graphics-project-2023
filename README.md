# Computer Graphics Project 2023

This repository contains the final project developed for the **Computer Graphics** course (2023 Ed.) at **Politecnico di Milano**.

TODO Course Description

TODO Project Goal

TODO Project Structure

## :volcano: Vulture

Vulture is our toy **Vulkan** game engine written on top of the famous [Vulkan Tutorial](https://vulkan-tutorial.com).

The **key features** of Vulture are:
- Multi-pipeline support
- Render varying amounts of dynamically instantiated objects
- **Tweens**, **Timers** and **asynchronous jobs**
- Dynamic **Skybox** support
- **UI** capabilities with **Image** and **Text** rendering
- A configurable **Input System** with M&K and Gamepad support


TODO Specific Descriptions

### :video_game: Input System

Vulture features a powerful input system that lets you define device-agnostic actions and associated bindings.

```cpp
InputAction fireAction{};

fireAction.keyboardBindings = {
  KeyboardBinding{{GLFW_KEY_SPACE}}
};
fireAction.mouseBindings = {
  MouseBinding{{GLFW_MOUSE_BUTTON_1}}
};
fireAction.gamepadButtonBindings = {
  GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER}},
  GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_X}}
};

Input::setAction("FIRE", fireAction);
```

You can also define more complex key combinations for your actions!

```cpp
InputAction runAction{};

runAction.keyboardBindings = {
  KeyboardBinding{{GLFW_KEY_W, GLFW_KEY_LEFT_SHIFT}}
};

Input::setAction("RUN", runAction);

```

After defining your actions, you can query the Input System with methods like `Input::isActionPressed`, `Input::isActionJustPressed`, `Input::isActionReleased`, or `Input::getActionStrength`.

### Why this name?

[Mount Vulture](https://en.wikipedia.org/wiki/Monte_Vulture) is an extinct Italian volcano! :volcano:


## :robot: Robot Survivor

**Robot Survivor** is a first person shooter in which you have to survive incoming waves of enemy robots on a procedurally generated alien planet.

TODO Game Screenshot

By destroying enemies, you can level up and obtain random upgrades to help you live longer!

Walking around, you'll be able to find powerful *pick-ups*!

TODO Pick-ups Screenshots

## License

This project is licensed under the [MIT License](LICENSE). Feel free to use, modify and distribute the code as per the terms of the license.
