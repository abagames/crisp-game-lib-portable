import * as view from "./view";

const letterSize = 6;
let textImages;
let charImages;

window.setTexts = (gridPointer, count) => {
  let p = gridPointer;
  textImages = [];
  for (let i = 0; i < count; i++) {
    textImages.push(createLetterImage(p));
    p += (letterSize + 1) * letterSize;
  }
};

window.setCharacters = (gridPointer, count) => {
  let p = gridPointer;
  charImages = [];
  for (let i = 0; i < count; i++) {
    charImages.push(createLetterImage(p));
    p += (letterSize + 1) * letterSize;
  }
};

window.text = (l, x, y) => {
  const i = l - "!".charCodeAt(0);
  view.context.drawImage(
    textImages[i],
    Math.floor(x - letterSize / 2),
    Math.floor(y - letterSize / 2)
  );
};

window.char = (l, x, y) => {
  const i = l - "a".charCodeAt(0);
  view.context.drawImage(
    charImages[i],
    Math.floor(x - letterSize / 2),
    Math.floor(y - letterSize / 2)
  );
};

let letterCanvas;
let letterContext;

export function init() {
  letterCanvas = document.createElement("canvas");
  letterCanvas.width = letterCanvas.height = letterSize;
  letterContext = letterCanvas.getContext("2d");
}

function createLetterImage(pointer) {
  letterContext.clearRect(0, 0, letterSize, letterSize);
  letterContext.fillStyle = "#000";
  let p = pointer;
  for (let y = 0; y < letterSize; y++) {
    for (let x = 0; x < letterSize; x++, p++) {
      const c = Module.HEAPU8[p];
      if (c === "l".charCodeAt(0)) {
        letterContext.fillRect(x, y, 1, 1);
      }
    }
    p++; // skip '\0'
  }
  const image = document.createElement("img");
  image.src = letterCanvas.toDataURL();
  return image;
}
