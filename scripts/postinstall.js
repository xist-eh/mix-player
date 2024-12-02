import { DEVMODE } from "../config.js";

import { rmSync } from "node:fs";

async function cleanDist() {
  rmSync("dist/win", { recursive: true, force: true });
  rmSync("dist/darwin", { recursive: true, force: true });
  rmSync("dist/linux", { recursive: true, force: true });
}

if (!DEVMODE) {
  cleanDist();
}
