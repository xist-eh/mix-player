import { existsSync, unlinkSync } from "fs";

import path from "path";

import addon from "./intermediary.js";
import { get } from "http";

function initializeMixPlayer() {
  addon.initAudioEngine();
}

function getPlaybackDevices() {
  return addon.getPlaybackDevices();
}

function setPlaybackDevice(deviceId) {
  return addon.setPlaybackDevice(deviceId);
}

function createAudio(filePath) {
  filePath = path.resolve(filePath);
  if (!existsSync(filePath)) {
    throw new Error(
      "File (" + filePath + ") used in createAudio() does not exist!"
    );
  }

  let isPlaying = false;
  let audioEndCallbacks = [];
  let awaitPromiseResolver = null;

  const audioIndex = addon.createNewSound(filePath);

  if (audioIndex === -1) {
    return null;
  }

  console.log("created audio with index:", audioIndex);

  const factory = {
    play: () => {
      isPlaying = true;
      addon.playSound(audioIndex);
    },
    pause: () => {
      isPlaying = false;

      if (awaitPromiseResolver) {
        awaitPromiseResolver();
        awaitPromiseResolver = null;
      }

      return addon.pauseSound(audioIndex);
    },
    getFilePath: () => {
      return filePath;
    },
    seek: (position) => {
      if (position < 0 || position > factory.getDuration()) {
        throw new Error("Invalid position");
      }
      return addon.seekSound(audioIndex, position);
    },
    setLooping: (loop) => {
      return addon.setSoundLooping(audioIndex, loop);
    },
    setVolume: (volume) => {
      return addon.setSoundVolume(audioIndex, volume);
    },
    getVolume: () => {
      return addon.getSoundVolume(audioIndex);
    },
    getDuration: () => {
      return addon.getSoundDuration(audioIndex);
    },
    getTrackPosition: () => {
      return addon.getSoundPosition(audioIndex);
    },
    getPitch: () => {
      return addon.getSoundPitch(audioIndex);
    },
    setPitch: (pitch) => {
      return addon.setSoundPitch(audioIndex, pitch);
    },
    getPan: () => {
      return addon.getSoundPan(audioIndex);
    },
    setPan: (pan) => {
      return addon.setSoundPan(audioIndex, pan);
    },
    onAudioEnd: (callback, callOnce = false) => {
      if (callOnce) {
        const index = audioEndCallbacks.push(() => {
          callback();
          audioEndCallbacks.splice(index, 1);
        });
        return;
      }
      audioEndCallbacks.push(callback);
    },
    destroy: () => {
      addon.destroySound(audioIndex);
      filePath = null;
      isPlaying = false;
      audioEndCallbacks = [];
      awaitPromiseResolver = null;
      for (const key of Object.keys(factory)) {
        delete factory[key];
      }
    },
    wait() {
      return new Promise((resolve, reject) => {
        const timeout = setTimeout(reject, 1000 * 60 * 60 * 24);
        awaitPromiseResolver = () => {
          clearTimeout(timeout);
          resolve();
        };
      });
    },
    isPlaying: () => {
      return isPlaying;
    },
  };

  addon.setAudioEndCallback(audioIndex, () => {
    for (let callback of audioEndCallbacks) {
      callback();
    }
    if (awaitPromiseResolver) {
      awaitPromiseResolver();
      awaitPromiseResolver = null;
    }
  });

  return factory;
}

const MixPlayerFactory = () => {
  initializeMixPlayer();
  return {
    createAudio,
    getPlaybackDevices,
    setPlaybackDevice,
  };
};

const MixPlayer = MixPlayerFactory();

export default MixPlayer;
