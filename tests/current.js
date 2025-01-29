import {
  createAudio,
  initializeMixPlayer,
  getPlaybackDevices,
} from "../binding.js";

initializeMixPlayer();

console.log(getPlaybackDevices());

const audio = createAudio("tests/test_audio.mp3");

const audio2 = createAudio("tests/test_audio.mp3");

audio2.destroy();

console.log(audio2);

audio.play();

await audio.wait();

console.log("Duration:", audio.getDuration());

setTimeout(() => {
  console.log("Hello 1");
  audio.pause();
  audio2.play();
  setTimeout(() => {
    console.log("Hello 2");
    audio.play();

    setTimeout(() => {
      console.log("Bye");
    }, 5000);
  }, 2000);
}, 2000);
