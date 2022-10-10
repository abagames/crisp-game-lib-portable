import * as view from "./view";
import * as character from "./character";
import * as input from "./input";
import * as audio from "./audio";

function update() {
  input.update();
  Module.ccall(
    "setInput",
    "void",
    ["boolean", "boolean", "boolean"],
    [input.isPressed, input.isJustPressed, input.isJustReleased]
  );
  Module.ccall("updateFrame", "void", [], []);
}

const targetFps = 68;
const deltaTime = 1000 / targetFps;
let nextFrameTime = 0;

function animationLoop() {
  requestAnimationFrame(animationLoop);
  const now = window.performance.now();
  if (now < nextFrameTime - targetFps / 12) {
    return;
  }
  nextFrameTime += deltaTime;
  if (nextFrameTime < now || nextFrameTime > now + deltaTime * 2) {
    nextFrameTime = now + deltaTime;
  }
  update();
}

function onRuntimeInitialized() {
  character.init();
  input.init();
  audio.init();
  Module.ccall("initGame", "void", [], []);
  animationLoop();
}

window.initView = (w, h) => {
  view.init(w, h);
};

window.Module = {
  onRuntimeInitialized,
};

// Load wasm
const scriptElement = document.createElement("script");
scriptElement.setAttribute("src", "./wasm/cglp.js");
document.head.appendChild(scriptElement);
