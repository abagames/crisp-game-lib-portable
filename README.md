# crisp-game-lib-portable

Minimal C-lang library for creating classic arcade-like mini-games running on devices and browsers. Re-implemented version of [crisp-game-lib](https://github.com/abagames/crisp-game-lib) for smaller devices.

## Target devices

- [M5StickC PLUS](https://shop.m5stack.com/products/m5stickc-plus-esp32-pico-mini-iot-development-kit)

![screenshot](./docs/screenshotM5StickCPlus.gif)

- [Adafruit PyBadge](https://learn.adafruit.com/adafruit-pybadge)

![screenshot](./docs/screenshotPyBadge.gif)

- Browser (built with [Emscripten](https://emscripten.org/))
  - [Play sample games](https://abagames.github.io/crisp-game-lib-portable/build/)

## Sample codes and reference

- [Sample codes](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/games)

- [Reference](https://abagames.github.io/crisp-game-lib-portable/ref_document/html/cglp_8c.html)

## Build for M5StickC PLUS

1. Install [LovyanGFX library](https://github.com/lovyan03/LovyanGFX).

1. Verify and upload [cglpM5StickCPlus.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpM5StickCPlus/cglpM5StickCPlus.ino) with [Arduino IDE 1.8](https://www.arduino.cc/en/software) or [vscode-arduino](https://github.com/microsoft/vscode-arduino).

1. To enable / disable sounds, press the B button.

1. Hold down the A button and press the B button to open the game selection menu.

## Build for PyBadge

1. Install [LovyanGFX library](https://github.com/lovyan03/LovyanGFX).

1. Verify and upload [cglpPyBadge.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpPyBadge/cglpPyBadge.ino) with [Arduino IDE 1.8](https://www.arduino.cc/en/software) or [vscode-arduino](https://github.com/microsoft/vscode-arduino).

1. To enable / disable sounds, press the START button.

1. Press the SELECT button to open the game selection menu.

## Build for browsers

1. Run `dev` npm script to start the dev server and watch [js files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/browser)

1. Run `dev_c` npm script to watch [c files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/lib) and build [wasm files](https://github.com/abagames/crisp-game-lib-portable/tree/main/public/wasm)

1. Button assignments: (A) X key, (B) Z key, (left/right/up/down) arrow keys

1. Press the X key while holding down the up and down arrow keys to open the game selection menu.
