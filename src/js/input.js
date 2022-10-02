export let isPressed = false;
export let isJustPressed = false;
export let isJustReleased = false;
let isDown = false;
let isClicked = false;
let isReleased = false;

export function init() {
  document.addEventListener("mousedown", (e) => {
    onDown(); //e.pageX, e.pageY);
  });
  document.addEventListener("touchstart", (e) => {
    onDown(); //e.touches[0].pageX, e.touches[0].pageY);
  });
  document.addEventListener("keydown", (e) => {
    onDown();
  });
  document.addEventListener("mouseup", (e) => {
    onUp();
  });
  document.addEventListener(
    "touchend",
    (e) => {
      e.preventDefault();
      e.target.click();
      onUp();
    },
    { passive: false }
  );
  document.addEventListener("keyup", (e) => {
    onUp();
  });
}

export function update() {
  isJustPressed = !isPressed && isClicked;
  isJustReleased = isPressed && isReleased;
  isPressed = isDown;
  isClicked = isReleased = false;
}

function onDown() {
  isDown = isClicked = true;
}

function onUp() {
  isDown = false;
  isReleased = true;
}
