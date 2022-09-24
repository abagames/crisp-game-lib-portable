import wasmHmr from "./wasmHmr";
const { defineConfig } = require("vite");

/** @type {import('vite').UserConfig} */
const config = {
  base: "./",
  build: {
    outDir: "docs/build/"
  },
  plugins: [wasmHmr()],
};

module.exports = defineConfig(config);
