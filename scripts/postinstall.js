import { exec } from "child_process";

import { promisify } from "util";
const execute = promisify(exec);

const DEVMODE = true;

async function cleanDist() {
  await execute("rm -rf ./dist/darwin");
  await execute("rm -rf ./dist/win");
  await execute("rm -rf ./dist/linux");
}

if (!DEVMODE) {
  cleanDist();
}
