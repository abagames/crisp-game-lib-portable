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
  initAudio();
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

let audioContext;
let gain;

function initAudio() {
  audioContext = new (window.AudioContext || window.webkitAudioContext)();
  window.addEventListener("mousedown", resumeAudio);
  window.addEventListener("touchstart", resumeAudio);
  window.addEventListener("keydown", resumeAudio);
  gain = audioContext.createGain();
  gain.gain.value = 0.05;
  gain.connect(audioContext.destination);
}

window.playTone = (freq, duration, when) => {
  const oscillator = audioContext.createOscillator();
  oscillator.type = "square";
  oscillator.frequency.value = freq;
  oscillator.start(when);
  oscillator.stop(when + duration);
  oscillator.connect(gain); 
}

window.stopTone = () => {
  oscillator.stop();
}

window.getAudioTime = () => {
  return audioContext.currentTime;
}

function resumeAudio() {
  audioContext.resume();
}

window.Module = {
  onRuntimeInitialized,
};

const scriptElement = document.createElement("script");
scriptElement.setAttribute("src", "./wasm/cglp.js");
document.head.appendChild(scriptElement);
