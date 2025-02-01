import MixPlayer from "../binding.js";
import assert from "node:assert/strict";

console.log("Audio devices:", MixPlayer.getPlaybackDevices());

const audio = MixPlayer.createAudio("tests/test_audio.mp3");

audio.play();

audio.setVolume(-10);

console.log("Volume:", audio.getVolume());

assert.strictEqual(Math.round(audio.getDuration()), 7);

audio.onAudioEnd(() => {
  console.log("Audio ended! Now what?");
});

await audio.wait();

audio.destroy();
