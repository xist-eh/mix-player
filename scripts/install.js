import { copyFileSync } from "node:fs";
import { promisify } from "util";
import { exec } from "child_process";
const execute = promisify(exec);

let src_dist_folder;
if (process.platform === "win32" || process.platform === "cygwin") {
  src_dist_folder = "dist/win";
} else if (process.platform === "linux") {
  src_dist_folder = "dist/linux";
} else if (process.platform === "darwin") {
  src_dist_folder = "dist/darwin";
}

await execute("npm run build");
copyFileSync(
  src_dist_folder + "/mix-player-native.node",
  "./dist/mix-player-native.node"
);
