# crisp-game-lib-portable ([DEMO](https://abagames.github.io/crisp-game-lib-portable/build/))

Minimal C-lang library for creating classic arcade-like mini-games running on devices and browsers (Experimental). Re-implemented version of [crisp-game-lib](https://github.com/abagames/crisp-game-lib) for smaller devices.

The goal now is to get it running on the [M5StickC PLUS](https://shop.m5stack.com/products/m5stickc-plus-esp32-pico-mini-iot-development-kit).

![screenshot](./docs/screenshot.gif)

[Emscripten](https://emscripten.org/) also makes it possible to run [games written in C](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/games/thunder.c) on the browser.

## Build for M5StickC PLUS

1. Install [LovyanGFX library](https://github.com/lovyan03/LovyanGFX).

1. Verify and upload [cglpM5StickCPlus.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpM5StickCPlus/cglpM5StickCPlus.ino) with [Arduino IDE 1.8](https://www.arduino.cc/en/software) or [vscode-arduino](https://github.com/microsoft/vscode-arduino).

1. To enable / disable sounds, press the B button.

1. Hold down the A button and press the B button to open the game select menu.

## Build for browsers

1. Run `dev` npm script to start the dev server and watch [js files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/browser)

1. Run `dev_c` npm script to watch [c files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/lib) and build [wasm files](https://github.com/abagames/crisp-game-lib-portable/tree/main/public/wasm)

1. Press the X key while holding down the up and down arrow keys to open the game select menu.

## Unimplemented features

- ~~Procedurally generated musics and [sound effects](https://abagames.github.io/crisp-game-lib/ref_document/functions/play.html)~~
- ~~[Difficulty](https://abagames.github.io/crisp-game-lib/ref_document/variables/difficulty.html)~~
- ~~[Score](https://abagames.github.io/crisp-game-lib/ref_document/functions/addScore.html)~~
- ~~[Game over](https://abagames.github.io/crisp-game-lib/ref_document/functions/end.html) and title screen~~
- ~~[Particle](https://abagames.github.io/crisp-game-lib/ref_document/functions/particle.html)~~
- ~~[Bar](https://abagames.github.io/crisp-game-lib/ref_document/functions/bar.html) and [box](https://abagames.github.io/crisp-game-lib/ref_document/functions/box.html)~~
- ~~[Color](https://abagames.github.io/crisp-game-lib/ref_document/functions/color.html)~~
- ~~Precise collision detection for texts and characters~~
- ~~[Letter options](https://abagames.github.io/crisp-game-lib/ref_document/types/LetterOptions.html)~~
  - `scale` and `backgroundColor` options are not supported
- ~~[Game options](https://abagames.github.io/crisp-game-lib/ref_document/types/Options.html)~~
  - Only `viewSize`, `soundSeed` and `isDarkColor` are supported
- ~~Utility functions~~
  - Partially supported
- ~~Replay~~
- ~~[Input position](https://abagames.github.io/crisp-game-lib/ref_document/variables/input.pos.html)~~
  - There is no plan to support a device with a touch panel
- ~~Support for multiple button types (Left/Right/Up/Down/B/A)~~
- ~~Menu screen with multiple game selections~~
- Support for another device (e.g. Adafruit PyBadge)
