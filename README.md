# crisp-game-lib-portable

Minimal C-lang library for creating classic arcade-like mini-games running on devices and browsers. Re-implemented version of [crisp-game-lib](https://github.com/abagames/crisp-game-lib) for smaller devices.

## Target devices

- [M5StickC PLUS](https://docs.m5stack.com/en/core/m5stickc_plus)

![screenshot](./docs/screenshotM5StickCPlus.gif)

- [Adafruit PyBadge](https://learn.adafruit.com/adafruit-pybadge)

![screenshot](./docs/screenshotPyBadge.gif)

- [M5Stack BASIC](http://docs.m5stack.com/en/core/basic)

  - [cglpM5Stack.ino](https://gist.github.com/obono/1606cf8a8a4e9c9f97de4ebebad3460a) (ported by [OBONO](https://github.com/obono))

- Browser (built with [Emscripten](https://emscripten.org/))
  - [Play sample games](https://abagames.github.io/crisp-game-lib-portable/build/)

## Sample codes and reference

- [Sample codes](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/games)

- [Reference](https://abagames.github.io/crisp-game-lib-portable/ref_document/html/cglp_8c.html)

## Build for M5StickC PLUS

1. Install [LovyanGFX library](https://github.com/lovyan03/LovyanGFX).

1. Create `cglpM5StickCPlus/` directory.

1. Copy [cglpM5StickCPlus.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpM5StickCPlus/cglpM5StickCPlus.ino), [./src/lib/\*](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/lib) and [./src/games/\*](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/games) files to the directory.

1. Verify and upload `cglpM5StickCPlus.ino` with [Arduino IDE](https://www.arduino.cc/en/software).

1. To enable / disable sounds, press the B button.

1. Hold down the A button and press the B button to open the game selection menu.

## Build for PyBadge

1. Install [LovyanGFX library](https://github.com/lovyan03/LovyanGFX).

1. Create `cglpPyBadge/` directory.

1. Copy [cglpPyBadge.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpPyBadge/cglpPyBadge.ino), [./src/lib/\*](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/lib) and [./src/games/\*](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/games) files to the directory.

1. Verify and upload `cglpPyBadge.ino` with [Arduino IDE](https://www.arduino.cc/en/software).

1. To enable / disable sounds, press the START button.

1. Press the SELECT button to open the game selection menu.

## Build for browsers

1. Run `dev` npm script to start the dev server and watch [js files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/browser)

1. Run `dev_c` npm script to watch [c files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/lib) and build [wasm files](https://github.com/abagames/crisp-game-lib-portable/tree/main/public/wasm)

1. Button assignments: (A) X key, (B) Z key, (left/right/up/down) arrow keys

1. Press the X key while holding down the up and down arrow keys to open the game selection menu.

## How to port the library to other devices

The source codes for [library](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/lib) and [games](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/games) are written device-independent. Besides, you need to implement device-dependent code for the following functions:

- Device initialization function (e.g. `setup()` in Arduino) that calls `initGame()`

- Frame update function (e.g. `loop()` in Arduino) that calls `setButtonState()` and `updateFrame()`

  - The state of the button press must be notified to the library with the `setButtonState()`

- Drawing and audio processing functions that are defined in [machineDependent.h](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/lib/machineDependent.h)
  - `md_getAudioTime()` function should return the audio timer value in seconds
  - `md_playTone(float freq, float duration, float when)` function should play a tone with `freq` frequency, `duration` length (in seconds) and staring from `when` seconds on the audio timer
  - `md_drawCharacter(unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3], float x, float y, int hash)` function should draw the pixel art defined by `grid[y][x][r, g, b]` at position (x, y). Since `hash` will be the same for the same pixel art, you can cache pixel art images using `hash` as an index and avoid redrawing the same image

Sample device-dependent codes are [cglpM5StickCPlus.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpM5StickCPlus/cglpM5StickCPlus.ino) and [cglpPyBadge.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpPyBadge/cglpPyBadge.ino).
