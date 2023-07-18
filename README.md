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
- A functional **Collision System** with collisions and layer masks support

### :sunny: User Interfaces

Vulture provides the tools to create simple User Interfaces.

To add some text, just run:

```cpp
Ref<UIText> text = Application::getScene()->getUIHandler()->makeText("My Text");
```

Text can be customized however you like: you can change its font, color, size, visibility and much more; and you can even animate it thanks to Vulture's **Tweens**!

To add an image, it works exactly the same way:

```cpp
Ref<UIImage> image = Application::getScene()->getUIHandler()->makeImage(imageName);
```

### :cloud: Dynamic Skybox

Setting up a Skybox with Vulture is easy! Just place your cubemap texture files in a `res/textures` folder and run the following code:

```cpp
Application::getScene()->setSkybox(skyboxName);
```

Your skybox will be asynchronously loaded and rendered, and it can be easily swapped out at runtime by calling the same method again, but with a different skybox name!

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

### :collision: Collision System

Vulture provides a simple way to manage **hitboxes**!

Look at this example for a reference on how you can setup a hitbox for your game object:

```cpp
// Create a hitbox with a Capsule shape
Ref<HitBox> hitbox = makeRef<HitBox>(makeRef<CapsuleCollisionShape>(radius, height));

// Sets the hitbox transform
hitbox->transform = gameObject->transform

// Define the Layer and Collision masks
hitbox->layerMask = BitMask::BIT0;
hitbox->collisionMask = BitMask::BIT1 | BitMask::BIT2;

// Add a collision callback
hitbox->addCallback([] (const HitBoxEntered& e)) {
  // Handle collision
}

// Add the hitbox to the scene
Application::getScene()->addHitbox();
```

Moreover, hitboxes can hold arbitrary data that can be accessed from objects that collide with them to implement more complex logic.

```cpp
// When initializing the hitbox
int damage = 1;
hitbox->data = &damage;

// When handling collision with that hitbox
void onHitboxEntered(const HitBoxEntered& event) {
  int damage = *reinterpret_cast<int*>(event.data);
  // Handle collision
}
```

Finally, to remove a hitbox, simply run `Application::getScene()->removeHitbox(hitbox)`.

*Note*: As of right now, Vulture only implements Capsule collision shapes, but more can be easily added.

### :hourglass_flowing_sand: Tweens, Timers and Job

**Tweens** are a very lightweight and powerful tool to animate *everything*.

TODO Tween Example + GIF 

**Timers** provide a simple API to run actions after a certain amount of time.

```cpp
// Print "Hello, World" after 1 second
Application::getScene()->makeTimer(1.0f)->addCallback([] (TimerTimeoutEvent e) {
  std::cout << "Hello, World!" << std::endl;
});
```

You can also define looping timers by explicitly passing a second argument `oneShot` to `Scene::makeTimer`.

**Jobs** can be used to run asynchronous long running tasks that may span multiple frames.

You can submit a new job by providing a function to run asynchronously and a function to run on the main thread once the task has finished executing.

```cpp
Job::submit(longRunningTask, data, cleanupCallback);
```

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
