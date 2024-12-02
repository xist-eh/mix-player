import { existsSync, unlinkSync } from "fs";

import path from "path";

import addon from "./intermediary.js";

function MixPlayer() {
  let pWaitResolve;
  let loop = false;

  let callbackFns = [];

  const player = {
    /**
     *
     * @param {Number} ms Milliseconds to fade audio in
     * @description Sets the time period to fade newly played audio in before reaching full volume
     */
    setFadeIn: (ms) => {
      addon.setFadeInPeriod(ms);
    },
    /**
     * @description Loads and plays an audio file from supported file types
     * @param {PathLike} file
     */
    play: (file) => {
      if (!file) {
        throw new Error("Expected string for MixPlayer.load()");
      }
      file = path.resolve(file);
      if (!existsSync(file)) {
        throw new Error("Specified file does not exist!");
      }

      addon.loadAudioFile(file);
    },
    /**
     * @description Rewinds track to beginning.
     */
    rewind: () => {
      addon.rewindAudio();
    },
    /**
     * @description Resumes paused audio
     */
    resume: () => {
      addon.playAudio();
    },
    /**
     * @description pauses playing audio. Resolves promise from MixPlayer.wait().
     */
    pause: () => {
      addon.pauseAudio();
      if (pWaitResolve) {
        pWaitResolve();
      }
    },
    /**
     *
     * @returns Returns a list of playback device names. First element is the default playback device
     */
    getAudioDevices: () => {
      return addon.getAudioDevices();
    },
    /**
     *
     * @param {Number} index Index of playback device from MixPlayer.getAudioDevice() to output to
     * @description Sets the output playback device. Note: If a selected playback device disconnects, then no audio is output
     */
    setPlaybackDevice: (index) => {
      addon.setAudioDevice(index);
    },
    /**
     *
     * @param {Number} vol Volume from 0 - 128
     * @description Sets the playback volume
     */
    setVolume: (vol) => {
      addon.setVolume(vol);
    },
    /**
     *
     * @returns Returns total duration of current audio
     */
    getAudioDuration: () => {
      return addon.getAudioDuration();
    },
    /**
     *
     * @param {Number} time Time to seek to
     * @description seeks current audio to specified time
     */
    seek: (time) => {
      addon.seekAudio(time);
    },
    /**
     *
     * @returns Promise that resolves when audio ends or is paused
     */
    wait: () => {
      if (!addon.isAudioPlaying()) {
        return new Promise((pResolve, pReject) => pResolve());
      }

      return new Promise((pResolve, pReject) => {
        pWaitResolve = pResolve;
      });
    },
    /**
     * @description Switches the option to loop current audio when it ends
     * @param {Boolean} bool Boolean on whether on not to loop audio
     */
    loop: (bool) => {
      loop = bool;
    },
    /**
     * @description A callback that fires when audio ends
     * @param {Function} fn
     */
    onAudioEnd: (fn) => {
      if (typeof fn !== "function") {
        throw new Error("expected fn");
      }
      callbackFns.push(fn);
    },
    /**
     * @description Destroys audio instance so program can safely exit
     */
    destroy: () => {
      pWaitResolve = null;
      addon.pauseAudio();
      addon.destroySDL();
    },
  };

  const audioCallback = () => {
    for (const i of callbackFns) {
      i();
    }
    if (loop == true) {
      console.log("looping..");
      player.rewind();
    }
    if (pWaitResolve && loop === false) {
      pWaitResolve();
      pWaitResolve = null;
    }
  };

  addon.onAudioEnd(audioCallback);

  return player;
}

const iMixPlayer = MixPlayer();

export { iMixPlayer as MixPlayer };
