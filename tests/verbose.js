import { MixPlayer } from "../binding.js";

let numPlayed = 0;

console.log("Playing...");

MixPlayer.play("tests/test_audio.mp3");

MixPlayer.onAudioEnd(() => {
  if (numPlayed !== 0) {
    MixPlayer.destroy();
    console.log("Bye Bye!");
    process.exit();
  }
  console.log("Rewinding...");
  numPlayed++;

  MixPlayer.setFadeIn(1500);
  MixPlayer.rewind();

  setTimeout(() => {
    console.log("pausing for a bit...");
    MixPlayer.pause();
  }, 3000);
  setTimeout(() => {
    console.log(
      "Resuming from",
      MixPlayer.getAudioPosition(),
      "seconds, Enjoy!"
    );
    MixPlayer.resume();
  }, 6000);
});
