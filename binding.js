import { existsSync, unlinkSync } from "fs";

import path from "path";

import addon from "./intermediary.js";

function MixPlayer() {
  let isDestroyed = false;

  let pWaitResolve;
  let loop = false;

  let callbackFns = [];

  const player = {
    /**
     *
     * @param {Number} ms Milliseconds to fade audio in
     * @description Sets the time period to fade newly played audio in before reaching full volume
     */
    setFadeIn: (msTime) => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      addon.setFadeInPeriod(msTime);
    },
    /**
     * @description Loads and plays an audio file from supported file types
     * @param {PathLike} file
     */
    play: (file) => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      if (!file) {
        throw new Error("Expected string for MixPlayer.load()");
      }
      file = path.resolve(file);
      if (!existsSync(file)) {
        throw new Error("Specified file does not exist!");
      }

      addon.loadAudioFile(file);
    },
    isPlaying: () => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      return addon.isAudioPlaying();
    },
    /**
     * @description Rewinds track to beginning.
     */
    rewind: () => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      addon.rewindAudio();
    },
    /**
     * @description Resumes paused audio
     */
    resume: () => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      addon.playAudio();
    },
    /**
     * @description pauses playing audio. Resolves promise from MixPlayer.wait().
     */
    pause: () => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      addon.pauseAudio();
      if (pWaitResolve) {
        pWaitResolve();
      }
    },
    /**
     *
     * @returns Returns a list of playback device names. First element is the default playback device
     */
    getOutputDevices: () => {
      return addon.getAudioDevices();
    },
    /**
     *
     * @param {Number} index Index of playback device from MixPlayer.getAudioDevice() to output to
     * @description Sets the output playback device. Note: If a selected playback device disconnects, then no audio is output
     */
    setPlaybackDevice: (index) => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      addon.setAudioDevice(index);
    },
    /**
     *
     * @param {Number} vol Volume from 0 - 128
     * @description Sets the playback volume
     */
    setVolume: (vol) => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      addon.setVolume(vol);
    },
    getVolume: () => {
      return addon.getVolume();
    },
    /**
     *
     * @returns Returns total duration of current audio
     */
    getAudioDuration: () => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      return addon.getAudioDuration();
    },
    /**
     *
     * @param {Number} time Time to seek to
     * @description seeks current audio to specified time
     */
    seek: (secondsTime) => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      if (secondsTime < 0) {
        throw new Error("Time to seek to cannot be negative!");
      }
      if (secondsTime > player.getAudioDuration()) {
        throw new Error(
          "Time to seek to is greater than duration of the current audio!"
        );
      }
      addon.seekAudio(secondsTime);
    },
    /**
     *
     * @returns Get the time current position of audio stream, in seconds.
     */
    getAudioPosition: () => {
      return addon.getAudioPosition();
    },
    /**
     *
     * @returns Promise that resolves when audio ends or is paused
     */
    wait: () => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
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
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      loop = bool;
    },
    /**
     * @description A callback that fires when audio ends
     * @param {Function} fn
     */
    onAudioEnd: (fn) => {
      if (isDestroyed) {
        throw new Error("MixPlayer has been destroyed!");
      }
      if (typeof fn !== "function") {
        throw new Error("expected fn");
      }
      callbackFns.push(fn);
    },
    /**
     * @description Destroys audio instance so program can safely exit
     */
    destroy: () => {
      if (pWaitResolve) {
        pWaitResolve();
      }
      addon.destroySDL();
      isDestroyed = true;
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
