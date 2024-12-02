import { DEVMODE } from "../config.js";

import { rmSync } from "node:fs";

async function cleanDist() {
  if(process.platform !== "win32"){
    rmSync("dist/win", { recursive: true, force: true });
  }
  if(process.platform !== "darwin"){
    rmSync("dist/darwin", { recursive: true, force: true });
  }
  if(process.platform !== "linux"){
    rmSync("dist/linux", { recursive: true, force: true });
  }
}

if (!DEVMODE) {
  cleanDist();
}
