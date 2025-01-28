import { existsSync, unlinkSync } from "fs";

import path from "path";

import addon from "./intermediary.js";
import { get } from "http";

export function getPlaybackDevices() {
  return addon.getPlaybackDevices();
}

export function initializeMixPlayer() {
  addon.initAudioEngine();
}

export function createAudio(filePath) {
  filePath = path.resolve(filePath);
  if (!existsSync(filePath)) {
    throw new Error(
      "File (" + filePath + ") used in createAudio() does not exist!"
    );
  }
  const audioIndex = addon.createNewSound(filePath, 111, () => {
    console.log("HELLO");
  });

  if (audioIndex === -1) {
    return null;
  }

  console.log("created audio with index:", audioIndex);

  return {
    play: () => {
      addon.playSound(audioIndex);
    },
    pause: () => {
      addon.pauseSound(audioIndex);
    },
    getDuration: () => {
      return addon.getSoundDuration(audioIndex);
    },
  };
}
