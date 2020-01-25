SDL_KEYDOWN and SDL_KEYUP for debounce. SDL_GetKeyboardState updates when SDL_PollEvents is called

We make a class that wraps SDL input methods, with getters to read state

We have InputSystem, then MouseState and KeyboardState have
public:
  friend class InputSystem;
Then add a MouseState instance to InputSystem

SDL has relative mouse mode which resets position to the center of the window each frame, and reports relative position to the last frame. This lets you implement camera rotation with a mouse.

Scroll wheel is events only.

Controllers are more complicated because there are more analogue signals, there can be multiple controllers, and controllers can be swapped during gameplay. github.com/gabomdq/SDL_GameControllerDB community maintained controller map. You need to poll each button individually (loop over the enums for eg)

Analogue signals are 0 -> 32768 for triggers, and -32768 -> 32768 for thumbsticks (with 0 in the center)

Use dead zones to prevent phantom inputs when analogue values aren't at their zero, or theoretical maximum positions. Also common to normalise this to a float 0 -> 1 range, making sure to clamp it so it cannot be outside the 9=0 -> 1 range.

For a thumbstick, if the input is fully up and to the right, be sure you don't exceed your player's max speed because of sqrt(1^2 + 1^2) = 1.4

SDL has an event for removing controllers.

## Input Mapping
You want an input map so a key is not forever tied to a specific game action
