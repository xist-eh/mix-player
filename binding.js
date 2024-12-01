import { existsSync, unlinkSync } from "fs";

import path from "path";

import addon from "./intermediary.js";

function MixPlayer() {
  let pWaitResolve;
  let loop = false;

  let callbackFns = [];

  const audioCallback = () => {
    for (const i of callbackFns) {
      i();
    }
    if (loop == true) {
      player.load();
    }
    if (pWaitResolve && loop === false) {
      pWaitResolve();
      pWaitResolve = null;
    }
  };

  addon.onAudioEnd(audioCallback);

  const player = {
    load: (file = null) => {
      if (file) {
        file = path.resolve(file);
        if (!existsSync(file)) {
          throw new Error("Specified file does not exist!");
        }
      }

      addon.loadAudioFile(file);
    },
    play: () => {
      addon.playAudio();
    },
    pause: () => {
      addon.pauseAudio();
    },
    getAudioDevices: () => {
      return addon.getAudioDevices();
    },
    setPlaybackDevice: (index) => {
      addon.setAudioDevice(index);
    },
    setVolume: (num) => {
      addon.setVolume(num);
    },
    getAudioDuration: () => {
      return addon.getAudioDuration();
    },
    seek: (time) => {
      addon.seekAudio(time);
    },
    wait: () => {
      if (!addon.isAudioPlaying()) {
        return new Promise((pResolve, pReject) => pResolve());
      }

      return new Promise((pResolve, pReject) => {
        pWaitResolve = pResolve;
      });
    },
    loop: (bool) => {
      loop = bool;
    },
    onAudioEnd: (fn) => {
      if (typeof fn !== "function") {
        throw new Error("expected fn");
      }
      callbackFns.push(fn);
    },
  };

  return player;
}

const iMixPlayer = MixPlayer();

export { iMixPlayer as MixPlayer };
