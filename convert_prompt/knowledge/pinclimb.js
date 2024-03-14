// Write the game name to 'title'.
title = "PIN CLIMB";

// 'description' is displayed on the title screen.
description = `
[Hold] Stretch
`;

// User-defined characters can be written here.
characters = [];

// Configure game options.
options = {
  isPlayingBgm: true,
  isReplayEnabled: true,
  // If you want to play a different BGM or SE,
  // you can try changing the 'seed' value.
  seed: 400,
};

// (Optional) Defining the types of variables is useful for
// code completion and error detection.
/** @type {{angle: number, length: number, pin: Vector}} */
let bar;
/** @type {Vector[]} */
let pins;
let nextPinDist;
const barLength = 7;

// 'update()' is called every frame (60 times per second).
function update() {
  // 'ticks' counts the number of frames from the start of the game.
  if (!ticks) {
    // Initialize the game state here. (ticks === 0)
    pins = [vec(50, 0)]; // 'vec()' creates a 2d vector instance.
    nextPinDist = 10;
    bar = { angle: 0, length: barLength, pin: pins[0] };
  }
  // 'difficulty' represents the difficulty of the game.
  // The value of this variable is 1 at the beginning of the game and
  // increases by 1 every minute.
  let scr = difficulty * 0.02;
  if (bar.pin.y < 80) {
    scr += (80 - bar.pin.y) * 0.1;
  }
  // 'input.isJustPressed' is set to true the moment the button is pressed.
  if (input.isJustPressed) {
    // 'play()' plays the SE.
    play("select");
  }
  // 'input.isPressed' is set to true while the button is pressed.
  if (input.isPressed) {
    bar.length += difficulty;
  } else {
    bar.length += (barLength - bar.length) * 0.1;
  }
  bar.angle += difficulty * 0.05;
  // Draw a line connecting the coordinates of
  // the first argument and the second argument.
  line(bar.pin, vec(bar.pin).addWithAngle(bar.angle, bar.length));
  if (bar.pin.y > 98) {
    play("explosion");
    // Call 'end()' to end the game. (Game Over)
    end();
  }
  let nextPin;
  // 'remove()' passes the elements of the array of the first argument to
  // the function of the second argument in order and executes it.
  // If the function returns true, the element will be removed from the array.
  remove(pins, (p) => {
    p.y += scr;
    // Draw a box and check if it collides with other black rectangles or lines.
    if (box(p, 3).isColliding.rect.black && p !== bar.pin) {
      nextPin = p;
    }
    return p.y > 102;
  });
  if (nextPin != null) {
    play("powerUp");
    // Add up the score.
    // By specifying the coordinates as the second argument,
    // the added score is displayed on the screen.
    addScore(ceil(bar.pin.distanceTo(nextPin)), nextPin);
    bar.pin = nextPin;
    bar.length = barLength;
  }
  nextPinDist -= scr;
  while (nextPinDist < 0) {
    // 'rnd()' returns a random value.
    pins.push(vec(rnd(10, 90), -2 - nextPinDist));
    nextPinDist += rnd(5, 15);
  }
}
