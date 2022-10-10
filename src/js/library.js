mergeInto(LibraryManager.library, {
  md_drawRect: function (x, y, w, h, r, g, b) {
    drawRect(x, y, w, h, r, g, b);
  },
  md_drawCharacter: function (grid, x, y, hash) {
    drawCharacter(grid, x, y, hash);
  },
  md_clearView: function (r, g, b) {
    clearView(r, g, b);
  },
  md_clearScreen: function (r, g, b) {
    clearScreen(r, g, b);
  },
  md_playTone: function (freq, duration, when) {
    playTone(freq, duration, when);
  },
  md_stopTone: function () {
    stopTone();
  },
  md_getAudioTime: function () {
    return getAudioTime();
  },
  md_consoleLog: function (msg) {
    console.log(UTF8ToString(msg));
  },
});
