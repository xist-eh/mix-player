import { createRequire } from "module";
import { DEVMODE } from "./config.js";
import { getDistPath } from "./get_dist_folder.js";
import { existsSync } from "fs";
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
  let destination = getDistPath();

  if (!existsSync(destination)) {
    throw new Error("Addon not found! Please run npm run dist");
  }
  importAddon(destination);
}
export default addon;
