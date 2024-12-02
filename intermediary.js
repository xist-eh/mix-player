import { createRequire } from "module";
import { DEVMODE } from "./config.js";

let addon;

function importAddon(url) {
  if (addon) {
    throw new Error("Addon is already defined!");
  }
  addon = createRequire(import.meta.url)(url);
}

if (DEVMODE) {
  importAddon("./src/build/Release/mix-player-native");
} else {
  importAddon("./dist/mix-player-native");
}
export default addon;
