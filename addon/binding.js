import { existsSync } from "fs";
import { createRequire } from "module";
import path from "path";

const addon = createRequire(import.meta.url)(
  "./build/Release/mix-player-native"
);

function MixPlayer() {
  let pWaitResolve;

  let callbackFns = [];

  const audioCallback = () => {
    if (pWaitResolve) {
      pWaitResolve();
      pWaitResolve = null;
    }
    for (const i of callbackFns) {
      i();
    }
  };

  const player = {
    load: (file) => {
      file = path.resolve(file);
      console.log(file);
      if (!existsSync(file)) {
        throw new Error("Attempted to play non-existent file!");
      }
      addon.loadAudioFile(file);
      addon.onAudioEnd(audioCallback);
    },
    play: () => {
      addon.playAudio();
    },
    pause: () => {
      addon.pauseAudio();
    },
    seek: (time) => {
      addon.seekAudio(time);
    },
    onAudioEnd: (fn) => {
      if (typeof fn !== "function") {
        throw new Error(
          "expected function for onAudioEnd callback, got",
          typeof fn
        );
      }
      callbackFns.push(fn);
    },
    wait: () => {
      return new Promise((pResolve, pReject) => {
        pWaitResolve = pResolve;
      });
    },
  };

  return player;
}

const iMixPlayer = MixPlayer();

export { iMixPlayer as MixPlayer };
