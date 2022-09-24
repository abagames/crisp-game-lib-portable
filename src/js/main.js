import "crisp-game-lib";

function update() {
  Module.ccall(
    "setInput",
    "void",
    ["boolean", "boolean", "boolean"],
    [input.isPressed, input.isJustPressed, input.isJustReleased]
  );
  Module.ccall("updateFrame", "void", [], []);
}

let characters = [];

function onRuntimeInitialized() {
  Module.ccall("initGame", "void", [], []);
  init({ update, characters, options: { isSoundEnabled: false, isShowingScore: false} });
}

window.setCharacters = (gridPointer, count) => {
  let p = gridPointer;
  characters = [];
  for (let i = 0; i < count; i++) {
    let cs = "\n";
    for (let y = 0; y < 6; y++) {
      for (let x = 0; x < 6; x++) {
        cs += String.fromCharCode(Module.HEAPU8[p + x]);
      }
      p += 7;
      cs += "\n";
    }
    characters.push(cs);
  }
};

window.Module = {
  onRuntimeInitialized,
};

const scriptElement = document.createElement("script");
scriptElement.setAttribute("src", "./wasm/cglp.js");
document.head.appendChild(scriptElement);
