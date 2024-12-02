**A cross platform & feature-rich audio player for node.js**

# Unique features

- Cross platform - supports Windows, Linux, and MacOS
- Works out of the box - no external dependencies* (Some Linux distros may require additional installation)
- Supports FLAC, MP3, M4A, Ogg, VOC, and WAV files
- Advanced control of playback through seek, volume, pause, fade in, and callback configurability
- Allows control over playback device

# Basic usage

```javascript
import { MixPlayer } from "MixPlayer";

MixPlayer.play("test_audio.mp3");

MixPlayer.onAudioEnd(() => {
  console.log("Audio ended! Now what?");
});

await MixPlayer.wait();

process.exit(0);
```

# Documentation
