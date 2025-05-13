//Builds native addon and adds it to a dist folder

import { copyFileSync } from "node:fs";
import { execSync } from "child_process";
import path from "node:path";
import getDistFolder from "../get_dist_folder";

let destination_dist_folder = getDistFolder();

execSync("npm run build");
copyFileSync(
  path.resolve("./src/build/Release/mix-player-native.node"),
  path.join(destination_dist_folder + "/mix-player-native.node")
);
