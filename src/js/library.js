mergeInto(LibraryManager.library, {
  md_rect: function (x, y, w, h) {
    rect(x, y, w, h);
  },
  md_text: function (l, x, y) {
    text(l, x, y);
  },
  md_character: function (l, x, y) {
    char(l, x, y);
  },
  md_color: function (r, g, b) {
    setColor(r, g, b);
  },
  md_clearView: function (r, g, b) {
    clearView(r, g, b);
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

  md_setTexts: function (grid, count) {
    setTexts(grid, count);
  },
  md_setCharacters: function (grid, count) {
    setCharacters(grid, count);
  },

  md_consoleLog: function (msg) {
    console.log(UTF8ToString(msg));
  },
});
