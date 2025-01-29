import {
  createAudio,
  initializeMixPlayer,
  getPlaybackDevices,
} from "../binding.js";

initializeMixPlayer();

console.log(getPlaybackDevices());

const audio = createAudio("tests/test_audio.mp3");

audio.play();

console.log("Volume:", audio.getVolume());

audio.onAudioEnd(() => {
  console.log("Audio ended! Now what?");
});

await audio.wait();

audio.destroy();
