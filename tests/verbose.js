import { MixPlayer } from "../binding.js";

console.log("Audio playback devices:", MixPlayer.getAudioDevices());

MixPlayer.setVolume(10);

MixPlayer.play("tests/test_audio.mp3");

MixPlayer.onAudioEnd(() => {
  console.log("Finished audio for the first time");
  MixPlayer.setFadeIn(2500);
  MixPlayer.seek(MixPlayer.getAudioDuration() - 10);

  setTimeout(() => {
    MixPlayer.pause();
  }, 5000);
});
