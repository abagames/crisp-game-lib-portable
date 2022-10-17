let audioContext;
let gain;

export function init() {
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
};

window.stopTone = () => {};

window.getAudioTime = () => {
  return audioContext.currentTime;
};

function resumeAudio() {
  audioContext.resume();
}
