//Builds native addon and adds it to a dist folder

import { copyFileSync } from "node:fs";
import { execSync } from "child_process";
import path from "node:path";
import { getDistPath } from "../get_dist_folder";

execSync("npm run build");
copyFileSync(
  path.resolve("./src/build/Release/mix-player-native.node"),
  getDistPath()
);
