import { existsSync } from "fs";
import { createRequire } from "module";
import path from "path";

const addon = createRequire(import.meta.url)(
  "./build/Release/mix-player-native"
);

function MixPlayer() {
  const player = {
    load: (file) => {
      file = path.resolve(file);
      console.log(file);
      if (!existsSync(file)) {
        throw new Error("Attempted to play non-existent file!");
      }
      addon.loadAudioFile(file);
      addon.onAudioEnd(() => {
        console.log("THIS IS FROM JS!");
      });
    },
    play: () => {
      addon.playAudio();
    },
    pause: () => {
      addon.pauseAudio();
    },
  };

  return player;
}

const iMixPlayer = MixPlayer();

export { iMixPlayer as MixPlayer };
