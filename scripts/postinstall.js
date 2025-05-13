import { exec, execSync } from "node:child_process";
import { DEVMODE } from "../config.js";

import { rmSync } from "node:fs";

async function cleanDist() {
  if (process.platform !== "win32") {
    rmSync("dist/win", { recursive: true, force: true });
  }
  if (process.platform !== "linux") {
    rmSync("dist/linux", { recursive: true, force: true });
  }
}

if (!DEVMODE) {
  cleanDist();
  if (!process.platform === "win32" || !process.platform === "linux") {
    console.log(
      "Prebuilt binaries are not available for this platform. Building the native addon."
    );
    execSync("npm run dist");
  }
  rmSync("src", { recursive: true, force: true });
}
