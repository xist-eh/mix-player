import path from "path";

export default function getDistFolder() {
  if (process.platform === "win32" || process.platform === "cygwin") {
    return "dist/win";
  } else if (process.platform === "linux") {
    return "dist/linux";
  } else if (process.platform === "darwin") {
    return "dist/darwin";
  } else if (process.platform === "freebsd") {
    return "dist/freebsd";
  } else {
    return "dist/unknown";
  }
}

export function getDistPath() {
  return path.join(
    import.meta.dirname,
    "/",
    getDistFolder(),
    "/mix-player-native.node"
  );
}
