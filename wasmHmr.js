export default function wasmHmr() {
  return {
    name: "wasmHmr",
    handleHotUpdate({ file, server }) {
      if (file.endsWith(".wasm")) {
        server.ws.send({ type: "full-reload" });
      }
    },
  };
}
