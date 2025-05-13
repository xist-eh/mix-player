import { createRequire } from "module";
import { DEVMODE } from "./config.js";
import getDistFolder from "./get_dist_folder.js";
import { existsSync } from "fs";
import path from "path";

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
  let destination_dist_folder = getDistFolder();
  if (
    !existsSync(path.join(destination_dist_folder + "/mix-player-native.node"))
  ) {
    throw new Error("Addon not found! Please run npm run dist");
  }
  importAddon(path.join(destination_dist_folder + "/mix-player-native.node"));
}
export default addon;
