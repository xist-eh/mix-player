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
} else if(process.platform === "win32"){
  importAddon("./dist/win/mix-player-native");
}
else if(process.platform === "darwin"){
  importAddon("./dist/darwin/mix-player-native");
}
else if(process.platform === "linux"){
  importAddon("./dist/linux/mix-player-native");
}
else {
  throw new Error("Precompiled binaries for Mix Player do not exist for your platform!");
}
export default addon;
