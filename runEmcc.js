const { readdirSync } = require("fs");
const { exec } = require("child_process");

const libFiles = readdirSync("./src/lib")
  .filter((fn) => fn.endsWith(".c"))
  .map((fn) => `./src/lib/${fn}`)
  .join(" ");
const gamesFiles = readdirSync("./src/games")
  .filter((fn) => fn.endsWith(".c"))
  .map((fn) => `./src/games/${fn}`)
  .join(" ");
const options =
  "-I./src/lib -O2 -sWASM=1 -sEXPORTED_RUNTIME_METHODS=ccall --js-library ./src/browser/library.js";
const output = "-o ./public/wasm/cglp.js";

const command = `emcc ${libFiles} ${gamesFiles} ${options} ${output}`;
exec(command, { encoding: "UTF-8" }, (error, stdout, stderr) => {
  if (error) {
    const eeIndex = stderr.indexOf("emcc: error:");
    if (eeIndex > 0) {
      console.log(stderr.substring(0, eeIndex - 1));
    } else {
      console.log(stderr);
    }
  }
});
