import { MixPlayer } from "../binding.js";
import assert from "node:assert/strict";

console.log("Audio devices:", MixPlayer.getAudioDevices());

MixPlayer.play("tests/test_audio.mp3");

assert.strictEqual(Math.round(MixPlayer.getAudioDuration()), 7);

MixPlayer.onAudioEnd(() => {
  console.log("Audio ended! Now what?");
});

await MixPlayer.wait();

process.exit(0);
