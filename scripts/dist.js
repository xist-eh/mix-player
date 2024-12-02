//Builds native addon and adds it to a dist folder

import { copyFileSync } from "node:fs";
import { promisify } from "util";
import { execSync } from "child_process";

let destination_dist_folder;
if (process.platform === "win32" || process.platform === "cygwin") {
  destination_dist_folder = "dist/win";
} else if (process.platform === "linux") {
  destination_dist_folder = "dist/linux";
} else if (process.platform === "darwin") {
  destination_dist_folder = "dist/darwin";
}

execSync("npm run build");
copyFileSync(
  "./src/build/Release/mix-player-native.node",
  destination_dist_folder + "/mix-player-native.node"
);
