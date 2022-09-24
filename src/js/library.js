mergeInto(LibraryManager.library, {
  md_rect: function (x, y, w, h) {
    rect(x, y, w, h);
  },
  md_text: function (l, x, y) {
    text(String.fromCharCode(l), x, y);
  },
  md_character: function (l, x, y) {
    char(String.fromCharCode(l), x, y);
  },
  md_playTone: function (freq, duration) {},
  md_stopTone: function () {},

  md_setTexts: function (grid, count) {},
  md_setCharacters: function (grid, count) {
    setCharacters(grid, count);
  },

  md_consoleLog: function (msg) {
    console.log(UTF8ToString(msg));
  },
});
