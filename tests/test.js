import { MixPlayer } from "../addon/binding.js";

MixPlayer.load("D:\\Dev\\blessed-player\\test-music\\bound2.mp3");

MixPlayer.play();

MixPlayer.seek(220);

await MixPlayer.wait();

console.log("ending program");
