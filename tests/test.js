import { MixPlayer } from "../binding.js";

console.log(MixPlayer.getAudioDevices());

MixPlayer.load("tests/test_audio.mp3");

console.log(MixPlayer.getAudioDuration());

MixPlayer.loop(true);

MixPlayer.onAudioEnd(() => {
  console.log("Audio ended! Now what?");
});

await MixPlayer.wait();

process.exit(0);
