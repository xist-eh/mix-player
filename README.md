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

MixPlayer.play("test_audio.mp3");

MixPlayer.onAudioEnd(() => {
  console.log("Audio ended! Now what?");
});

await MixPlayer.wait();

process.exit(0);
```

# Documentation

## MixPlayer.play(PathLike file)

Plays a supported file type (FLAC, MP3, Ogg, VOC, and WAV). Will either use default system playback device or output device selected by MixPlayer.setPlaybackDevices().

## MixPlayer.pause()

Pauses current audio.

## MixPlayer.resume()

Resumes current audio.

## MixPlayer.rewind()

Plays current audio from beginning, even after it has ended.

## MixPlayer.seek(Number seconds)

Seeks audio to given time. Does not work if audio has ended.

## MixPlayer.setVolume(Number volume)

Sets playback volume. Volume ranges from 0 (silence) - 128 (full loudness). Default is 128.

## MixPlayer.getVolume() -> Number

Returns current volume.

## MixPlayer.isPlaying() -> Boolean

Returns true if audio is currently playing. False otherwise.

## MixPlayer.getAudioDuration() -> Number

Returns the total duration of current audio in seconds

## MixPlayer.getAudioPosition() -> Number

Get the time current position of audio stream, in seconds.

## MixPlayer.wait() -> Promise

Returns a Promise that resolves when audio ends, resolving immediately if none is playing. Also resolves if audio is paused or MixPlayer is destroyed.

## MixPlayer.onAudioEnd(Function callback)

Set a callback that fires when audio ends.

## MixPlayer.setFadeIn(Number milliseconds)

Sets the amount of time taken to go from silence to full volume. Default is 0 ms.

## MixPlayer.loop(bool shouldLoop)

Switches the option to loop current audio when it ends. If looping is enabled, then the promise from MixPlayer.wait() will not resolve after audio ends.

## MixPlayer.getOutputDevices() -> String[]

Returns an array of available playback device names. The first element is the default system playback device.

## MixPlayer.setPlaybackDevice(Number index)

Set the playback device to the device at given index of MixPlayer.getAudioDevices(). Note that if the specified device disconnects, then output device is not automatically changed.

## MixPlayer.destroy()

Destroy MixPlayer, uninitializing the player and removing the callback loop, to safely exit the program. MIXPLAYER CANNOT BE REINITIALIZED!
