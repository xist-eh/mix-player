**A cross platform & feature-rich audio player for node.js**

# Unique features

- Cross platform - supports Windows, Linux, and MacOS
- Works out of the box - no external dependencies\* (Some Linux distros may require additional installation) (MacOS binary is under development)
- Supports FLAC, MP3, Ogg, VOC, and WAV files
- Advanced control of playback through seek, volume, pause, fade in, and callback configurability
- Allows control over playback device

# Basic usage

```javascript
import { MixPlayer } from "MixPlayer";

const audio = MixPlayer.createAudio("test_audio.mp3");

MixPlayer.onAudioEnd(() => {
  console.log("Audio ended! Now what?");
});

audio.play();

await audio.wait();

audio.destroy();
```

# Documentation

## MixPlayer.createAudio(PathLike file): Object | null

Creates an audio object if file type is supported (MP3, FLAC, WAV). Methods are below.

### audio.play()

Plays audio from current track position

### audio.pause()

Pauses audio playback

### audio.isPlaying(): Boolean

Returns true if audio is playing

### audio.getDuration(): Number

returns the total duration of the audio in seconds

### audio.seek(Number position)

Sets the track position in seconds

### audio.getTrackPosition: Number

Returns the current track position in seconds

### audio.setVolume(Number volume)

Sets the playback volume in decibels. A positive integer increases the volume (e.g 10) while a negative integer decreases it (e.g -10)

### audio.getVolume(): Number

Returns the volume change in db

### audio.wait(): Promise

Returns a Promise that resolves when audio ends, resolving immediately if paused.

### audio.onAudioEnd(Function callback, Boolean callOnce = false)

Add a callback that fires once audio ends. If callOnce is set to true, callback is only fired the first time audio ends.

### audio.setLooping(boolean loop)

If true, audio loops after end. NOTE: wait and onAudioEnd are not fired.

### audio.destroy()

Destroys audio object and frees memory

## MixPlayer.getPlaybackDevices(): Object[]

Returns an array of objects on playback device information.

Keys: id (Number), name (String), isDefault (boolean)

## MixPlayer.setPlaybackDevice(Number deviceId)

Sets the playback device to id, which can be found using
