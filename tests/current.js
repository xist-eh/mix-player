import MixPlayer from "../binding.js";

const audio = MixPlayer.createAudio("tests/test_audio.mp3");

audio.play();

audio.onAudioEnd(() => {
  console.log("Audio ended! Now what?");
});

await audio.wait();

audio.seek(0);

audio.play();

await audio.wait();

console.log("Audio ended! its the end");
