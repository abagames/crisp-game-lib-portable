import * as view from "./view";

const letterSize = 6;
let textImages;
let charImages;

window.setTexts = (gridPointer, count) => {
  let p = gridPointer;
  textImages = [];
  for (let i = 0; i < count; i++) {
    textImages.push(createLetterImage(p));
    p += letterSize * letterSize * 3;
  }
};

window.setCharacters = (gridPointer, count) => {
  let p = gridPointer;
  charImages = [];
  for (let i = 0; i < count; i++) {
    charImages.push(createLetterImage(p));
    p += letterSize * letterSize * 3;
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
  let p = pointer;
  for (let y = 0; y < letterSize; y++) {
    for (let x = 0; x < letterSize; x++, p += 3) {
      const r = Module.HEAPU8[p];
      const g = Module.HEAPU8[p + 1];
      const b = Module.HEAPU8[p + 2];
      if (r > 0 || g > 0 || b > 0) {
        letterContext.fillStyle = view.rgbToColorStyle(r, g, b);
        letterContext.fillRect(x, y, 1, 1);
      }
    }
  }
  const image = document.createElement("img");
  image.src = letterCanvas.toDataURL();
  return image;
}
