# crisp-game-lib-portable

Minimal C-lang library for creating classic arcade-like mini-games running on devices and browsers (Experimental). Re-implemented version of [crisp-game-lib](https://github.com/abagames/crisp-game-lib) for smaller devices.

The goal now is to get it running on the [M5StickC PLUS](https://shop.m5stack.com/products/m5stickc-plus-esp32-pico-mini-iot-development-kit).

<a href="./docs/screenshot.gif"><img src="./docs/screenshot.gif" width="25%"></a>

[Emscripten](https://emscripten.org/) also makes it possible to run [games written in C](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/games/survivor.c) on the browser.

- [SURVIVOR](https://abagames.github.io/crisp-game-lib-portable/build/survivor/)
- [PIN CLIMB](https://abagames.github.io/crisp-game-lib-portable/build/pinclimb/)
- [THUNDER](https://abagames.github.io/crisp-game-lib-portable/build/thunder/)

## Build for M5StickC PLUS

1. Install [LovyanGFX library](https://github.com/lovyan03/LovyanGFX).

1. Compile [m5stickcplus.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/c/m5stickcplus.ino) with [Arduino IDE 1.8](https://www.arduino.cc/en/software) or [vscode-arduino](https://github.com/microsoft/vscode-arduino).

1. To enable / disable sounds, press the button B.

## Build for browsers

1. Run `dev` npm script to start the dev server and watch [js files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/js)

1. Run `dev_c` npm script to watch [c files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/c) and build [wasm files](https://github.com/abagames/crisp-game-lib-portable/tree/main/public/wasm)

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
  -- `scale` and `backgroundColor` options are not supported
- ~~[Game options](https://abagames.github.io/crisp-game-lib/ref_document/types/Options.html)~~
  -- Only `viewSize`, `soundSeed` and `isDarkColor` are supported
- ~~Utility functions~~
  -- Partially supported
- Replay
- [Input position](https://abagames.github.io/crisp-game-lib/ref_document/variables/input.pos.html)
