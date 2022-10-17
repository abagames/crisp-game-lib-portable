export let canvas;
export let context;

export function init(w, h) {
  if (canvas != null) {
    document.body.removeChild(canvas);
    window.removeEventListener("resize", setSize);
  }
  const _bodyBackground = "#e0e0e0";
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
  window.addEventListener("resize", setSize);
  setSize();
}

function setSize() {
  const cs = 0.95;
  const wr = innerWidth / innerHeight;
  const cr = canvas.width / canvas.height;
  const flgWh = wr < cr;
  const cw = flgWh ? cs * innerWidth : cs * innerHeight * cr;
  const ch = !flgWh ? cs * innerHeight : (cs * innerWidth) / cr;
  canvas.style.width = `${cw}px`;
  canvas.style.height = `${ch}px`;
}

export function clear(r, g, b) {
  context.fillStyle = rgbToColorStyle(r, g, b);
  context.fillRect(0, 0, canvas.width, canvas.height);
}

window.clearView = (r, g, b) => {
  clear(r, g, b);
};

window.clearScreen = (r, g, b) => {
  document.body.style.background = rgbToColorStyle(r, g, b);
};

function intToHex(v) {
  return v.toString(16).padStart(2, "0");
}

export function rgbToColorStyle(r, g, b) {
  return `#${intToHex(r)}${intToHex(g)}${intToHex(b)}`;
}

export function fillRect(x, y, width, height, r, g, b) {
  context.fillStyle = rgbToColorStyle(r, g, b);
  context.fillRect(
    Math.floor(x),
    Math.floor(y),
    Math.trunc(width),
    Math.trunc(height)
  );
}

window.drawRect = (x, y, w, h, r, g, b) => {
  fillRect(x, y, w, h, r, g, b);
};
