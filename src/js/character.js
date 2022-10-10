import * as view from "./view";

const size = 6;
let images;
let canvas;
let context;

function createCharacterImage(grid) {
  context.clearRect(0, 0, size, size);
  let p = grid;
  for (let y = 0; y < size; y++) {
    for (let x = 0; x < size; x++, p += 3) {
      const r = Module.HEAPU8[p];
      const g = Module.HEAPU8[p + 1];
      const b = Module.HEAPU8[p + 2];
      if (r > 0 || g > 0 || b > 0) {
        context.fillStyle = view.rgbToColorStyle(r, g, b);
        context.fillRect(x, y, 1, 1);
      }
    }
  }
  const image = document.createElement("img");
  image.src = canvas.toDataURL();
  return image;
}

window.drawCharacter = (grid, x, y, hash) => {
  let im = images[hash];
  if (im == null) {
    im = createCharacterImage(grid);
    images[hash] = im;
  }
  view.context.drawImage(im, Math.floor(x), Math.floor(y));
};

export function init() {
  canvas = document.createElement("canvas");
  canvas.width = canvas.height = size;
  context = canvas.getContext("2d");
  images = {};
}
