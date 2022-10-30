# crisp-game-lib-portable

Minimal C-lang library for creating classic arcade-like mini-games running on devices and browsers. Re-implemented version of [crisp-game-lib](https://github.com/abagames/crisp-game-lib) for smaller devices.

## Target devices

- [M5StickC PLUS](https://docs.m5stack.com/en/core/m5stickc_plus)

![screenshot](./docs/screenshotM5StickCPlus.gif)

- [M5Stack BASIC](http://docs.m5stack.com/en/core/basic)

- [Adafruit PyBadge](https://learn.adafruit.com/adafruit-pybadge)

![screenshot](./docs/screenshotPyBadge.gif)

- Browser
  - [Play sample games](https://abagames.github.io/crisp-game-lib-portable/build/)

## Sample codes and reference

- [Sample codes](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/games)

- Reference
  - [Functions and variables](https://abagames.github.io/crisp-game-lib-portable/ref_document/html/cglp_8c.html)
  - [Structs and macros](https://abagames.github.io/crisp-game-lib-portable/ref_document/html/cglp_8h.html)
  - [2d vector functions](https://abagames.github.io/crisp-game-lib-portable/ref_document/html/vector_8c.html) ([macros](https://abagames.github.io/crisp-game-lib-portable/ref_document/html/vector_8h.html))

## How to write your own game

1. Copy [game_Template.c](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/games/game_Template.c) to `game[your game name].c`

1. Comment out other games in [menuGameList.c](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/lib/menuGameList.c) and add `addGame[your game name]()`

```
void addGames() {
  /*addGameThunder();
  ...(snip)...
  addGameReflector();*/
  addGame[your game name]();
}
```

1. Write your own game in `game[your game name].c` and rename `void addGame_Template() {` to `void addGame[your game name]() {`

1. [Build for browser](##Browser) and debug

1. Once the game is complete, revert other games that were commented out in [menuGameList.c](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/lib/menuGameList.c) and build it for other devices

## Build for [target device]

### M5StickCPlus, M5Stack, PyBadge

1. Install [LovyanGFX library](https://github.com/lovyan03/LovyanGFX)

1. Create `cglp[target device]/` directory. (e.g. `cglpM5StickCPlus/`)

1. Copy `cglp[target device].ino`, [./src/lib/\*](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/lib) and [./src/games/\*](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/games) files to the directory

- [cglpM5StickCPlus.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpM5StickCPlus/cglpM5StickCPlus.ino)
- [cglpM5Stack.ino](https://gist.github.com/obono/1606cf8a8a4e9c9f97de4ebebad3460a) (ported by [OBONO](https://github.com/obono))
- [cglpPyBadge.ino](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/cglpPyBadge/cglpPyBadge.ino)

1. Verify and upload `cglp[target device].ino` with [Arduino IDE](https://www.arduino.cc/en/software)

### Browser

1. Install [Emscripten](https://emscripten.org/)

1. Run `dev` npm script to start the dev server and watch [js files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/browser)

1. Run `dev_c` npm script to watch [c files](https://github.com/abagames/crisp-game-lib-portable/tree/main/src/lib) and build [wasm files](https://github.com/abagames/crisp-game-lib-portable/tree/main/public/wasm)

## How to operate

### Back to the game selection menu

- Hold down the A button and press the B button (M5StickCPlus, M5Stack)
- Press the SELECT button (PyBadge)
- Press the X key while holding down the up and down arrow keys (Browser)

### Toggle sound on/off

- Press the B button (M5StickCPlus)
- Press the C Button (M5Stack)
- Press the START button (PyBadge)
- Press the Z key while holding down the up and down arrow keys (Browser)

### Key assignment on browser

- (A) X key, (B) Z key, (left/right/up/down) arrow keys

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
