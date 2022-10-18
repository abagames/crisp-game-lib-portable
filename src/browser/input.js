export let isLeftPressed;
export let isRightPressed;
export let isUpPressed;
export let isDownPressed;
export let isBPressed;
export let isAPressed;
let isPointerDown;

export function init() {
  initKeyboard();
  isPointerDown = false;
  document.addEventListener("mousedown", (e) => {
    onDown();
  });
  document.addEventListener("touchstart", (e) => {
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
}

export function update() {
  updateKeyboard(isPointerDown);
}

function onDown() {
  isPointerDown = true;
}

function onUp() {
  isPointerDown = false;
}

let pressingCode;

export function initKeyboard() {
  isLeftPressed =
    isRightPressed =
    isUpPressed =
    isDownPressed =
    isBPressed =
    isAPressed =
      false;
  pressingCode = {};
  document.addEventListener("keydown", (e) => {
    pressingCode[e.code] = true;
    if (
      e.code === "AltLeft" ||
      e.code === "AltRight" ||
      e.code === "ArrowRight" ||
      e.code === "ArrowDown" ||
      e.code === "ArrowLeft" ||
      e.code === "ArrowUp"
    ) {
      e.preventDefault();
    }
  });
  document.addEventListener("keyup", (e) => {
    pressingCode[e.code] = false;
  });
}

export function updateKeyboard(isPointerPressed) {
  isLeftPressed = pressingCode["ArrowLeft"];
  isRightPressed = pressingCode["ArrowRight"];
  isUpPressed = pressingCode["ArrowUp"];
  isDownPressed = pressingCode["ArrowDown"];
  isBPressed = pressingCode["KeyZ"] || pressingCode["KeyB"];
  isAPressed = isPointerPressed;
  if (isAPressed) {
    return;
  }
  for (let i = 0; i < buttonACodes.length; i++) {
    if (pressingCode[buttonACodes[i]]) {
      isAPressed = true;
      break;
    }
  }
}

const buttonACodes = [
  "KeyX",
  "KeyA",

  "Escape",
  "Digit0",
  "Digit1",
  "Digit2",
  "Digit3",
  "Digit4",
  "Digit5",
  "Digit6",
  "Digit7",
  "Digit8",
  "Digit9",
  "Minus",
  "Equal",
  "Backspace",
  "Tab",
  "KeyQ",
  "KeyW",
  "KeyE",
  "KeyR",
  "KeyT",
  "KeyY",
  "KeyU",
  "KeyI",
  "KeyO",
  "KeyP",
  "BracketLeft",
  "BracketRight",
  "Enter",
  "ControlLeft",
  //"KeyA",
  "KeyS",
  "KeyD",
  "KeyF",
  "KeyG",
  "KeyH",
  "KeyJ",
  "KeyK",
  "KeyL",
  "Semicolon",
  "Quote",
  "Backquote",
  "ShiftLeft",
  "Backslash",
  //"KeyZ",
  //"KeyX",
  "KeyC",
  "KeyV",
  //"KeyB",
  "KeyN",
  "KeyM",
  "Comma",
  //"Period",
  //"Slash",
  "ShiftRight",
  "NumpadMultiply",
  "AltLeft",
  "Space",
  "CapsLock",
  "F1",
  "F2",
  "F3",
  "F4",
  "F5",
  "F6",
  "F7",
  "F8",
  "F9",
  "F10",
  "Pause",
  "ScrollLock",
  "Numpad7",
  "Numpad8",
  "Numpad9",
  "NumpadSubtract",
  "Numpad4",
  "Numpad5",
  "Numpad6",
  "NumpadAdd",
  "Numpad1",
  "Numpad2",
  "Numpad3",
  "Numpad0",
  "NumpadDecimal",
  "IntlBackslash",
  "F11",
  "F12",
  "F13",
  "F14",
  "F15",
  "F16",
  "F17",
  "F18",
  "F19",
  "F20",
  "F21",
  "F22",
  "F23",
  "F24",
  "IntlYen",
  "Undo",
  "Paste",
  "MediaTrackPrevious",
  "Cut",
  "Copy",
  "MediaTrackNext",
  "NumpadEnter",
  "ControlRight",
  "LaunchMail",
  "AudioVolumeMute",
  "MediaPlayPause",
  "MediaStop",
  "Eject",
  "AudioVolumeDown",
  "AudioVolumeUp",
  "BrowserHome",
  "NumpadDivide",
  "PrintScreen",
  "AltRight",
  "Help",
  "NumLock",
  "Pause",
  "Home",
  //"ArrowUp",
  "PageUp",
  //"ArrowLeft",
  //"ArrowRight",
  "End",
  //"ArrowDown",
  "PageDown",
  "Insert",
  "Delete",
  "OSLeft",
  "OSRight",
  "ContextMenu",
  "BrowserSearch",
  "BrowserFavorites",
  "BrowserRefresh",
  "BrowserStop",
  "BrowserForward",
  "BrowserBack",
];
