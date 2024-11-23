import { existsSync } from "fs";

import { createRequire } from "module";
import path from "path";

const mixPlayer = createRequire(import.meta.url)("./build/Release/mix-player");

function PlayerFactory() {
  let isPlaying = false;
  let autoPlay = true;
  let audioQueue = [];

  let loop = false;

  let callbackFns = [];

  const interval = setInterval(() => {
    if (!mixPlayer.hasAudioEnded()) {
      return;
    }

    completedFile = audioQueue.shift();

    if (factoryObject.getTrackDuration() > 1 && isPlaying) {
      callbackFns.forEach((fn) => fn(completedFile));
    }

    if (loop) {
      audioQueue.splice(1, 0, audioQueue.at(0));
    }

    if (audioQueue.length === 0) {
      isPlaying = false;
      return;
    }

    if ((autoPlay || loop) && isPlaying) {
      factoryObject.play();
    }
  }, 500);

  const factoryObject = {
    /**
     * @description destroys the player object
     */
    destroy: () => {
      clearInterval(interval);
    },
    /**
     *
     * @param {import("fs").PathLike} filePath path of audio file
     * @param {Number} position position in queue
     * @returns null
     * @description pushes file to audio queue
     */
    addToQueue: (filePath, pos = null) => {
      filePath = path.resolve(filePath);
      if (!existsSync(filePath)) {
        throw new Error("specified file does not exist");
      }
      if (pos === null) {
        audioQueue.push(filePath);
        return;
      }
      audioQueue.splice(pos, 0, filePath);
    },
    /**
     * @description removes specified item from audio queue
     * @param {Number} index index in audio queue to remove. Cannot be currently playing
     *
     */
    removeFromQueue: (index) => {
      audioQueue.splice(index, 1);
    },
    /**
     * @description pauses the currently playing audio
     */
    pause: () => {
      mixPlayer.pauseAudio();
      isPlaying = false;
    },
    /**
     *
     * @returns null
     * @description resumes paused audio or plays the next audio in queue after current has finished
     */
    play: () => {
      isPlaying = true;

      if (mixPlayer.hasAudioEnded() && audioQueue.length !== 0) {
        mixPlayer.playAudioFile(audioQueue.at(0));

        return;
      }
      mixPlayer.playAudio();
    },
    setTrackPosition: (pos) => {
      mixPlayer.setAudioPosition(pos);
    },
    getTrackDuration: () => {
      return mixPlayer.getAudioDuration();
    },
    loop: (bool) => {
      loop = bool;
    },
    autoPlay: (bool) => {
      autoPlay = bool;
    },
    onTrackEnd: (fn) => {
      callbackFns.push(fn);
    },
  };

  return factoryObject;
}

const instance = PlayerFactory();

export { instance as player };
