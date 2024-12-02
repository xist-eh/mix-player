import { copyFileSync } from "node:fs";

let src_dist_folder;
if (process.platform === "win32" || process.platform === "cygwin") {
  src_dist_folder = "dist/win";
} else if (process.platform === "linux") {
  src_dist_folder = "dist/linux";
} else if (process.platform === "darwin") {
  src_dist_folder = "dist/darwin";
}

copyFileSync(
  src_dist_folder + "/mix-player-native.node",
  "./dist/mix-player-native.node"
);
