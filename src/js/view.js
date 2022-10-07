export let canvas;
export let context;

export function init(w, h) {
  const _bodyBackground = "#ddd";
  const bodyCss = `
-webkit-touch-callout: none;
-webkit-tap-highlight-color: ${_bodyBackground};
-webkit-user-select: none;
-moz-user-select: none;
-ms-user-select: none;
user-select: none;
background: ${_bodyBackground};
color: #888;
`;
  const canvasCss = `
position: absolute;
left: 50%;
top: 50%;
transform: translate(-50%, -50%);
`;
  const crispCss = `
image-rendering: -moz-crisp-edges;
image-rendering: -webkit-optimize-contrast;
image-rendering: -o-crisp-edges;
image-rendering: pixelated;
`;
  document.body.style.cssText = bodyCss;
  canvas = document.createElement("canvas");
  canvas.width = w;
  canvas.height = h;
  context = canvas.getContext("2d");
  context.imageSmoothingEnabled = false;
  canvas.style.cssText = canvasCss + crispCss;
  document.body.appendChild(canvas);
  const setSize = () => {
    const cs = 0.95;
    const wr = innerWidth / innerHeight;
    const cr = canvas.width / canvas.height;
    const flgWh = wr < cr;
    const cw = flgWh ? cs * innerWidth : cs * innerHeight * cr;
    const ch = !flgWh ? cs * innerHeight : (cs * innerWidth) / cr;
    canvas.style.width = `${cw}px`;
    canvas.style.height = `${ch}px`;
  };
  window.addEventListener("resize", setSize);
  setSize();
}

let currentColorFillStyle;

export function clear(r, g, b) {
  const cc = currentColorFillStyle;
  setColor(r, g, b);
  context.fillStyle = currentColorFillStyle;
  context.fillRect(0, 0, canvas.width, canvas.height);
  currentColorFillStyle = cc;
}

window.clearView = (r, g, b) => {
  clear(r, g, b);
};

function intToHex(v) {
  return Math.floor(v * 255)
    .toString(16)
    .padStart(2, "0");
}

window.setColor = (r, g, b) => {
  currentColorFillStyle = `#${intToHex(r)}${intToHex(g)}${intToHex(b)}`;
};

export function fillRect(x, y, width, height) {
  context.fillStyle = currentColorFillStyle;
  context.fillRect(
    Math.floor(x),
    Math.floor(y),
    Math.trunc(width),
    Math.trunc(height)
  );
}

window.rect = (x, y, w, h) => {
  fillRect(x, y, w, h);
};
