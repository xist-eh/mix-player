import MixPlayer from "../binding.js";

const audio = MixPlayer.createAudio("tests/test_audio.mp3");

audio.play();

setTimeout(() => {}, 10000);
