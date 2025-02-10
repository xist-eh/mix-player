#define NAPI_VERSION 5

#define NODE_ADDON_API_CPP_EXCEPTIONS 1
#define MA_NO_ENCODING

#define MINIAUDIO_IMPLEMENTATION

#include "../node_modules/node-addon-api/napi.h"

#include "miniaudio.h"

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

ma_engine engine;
ma_uint32 sampleRate;

ma_context context;
ma_device_info *pPlaybackInfos;
ma_uint32 playbackDeviceCount;

class MixSound
{
private:
    std::string audioFilePath;
    ma_sound maMixSound;
    float duration;
    Napi::ThreadSafeFunction tsfnCallback = NULL;

    static void onAudioEnd(void *passed, ma_sound *pSound)
    {

        Napi::ThreadSafeFunction tsfn = *(Napi::ThreadSafeFunction *)passed;

        if (tsfn == NULL)
        {
            return;
        }
        tsfn.NonBlockingCall();
    }

public:
    MixSound(std::string audioPath)
    {
        audioFilePath = audioPath;

        ma_result result = ma_sound_init_from_file(&engine, audioFilePath.c_str(), 0, NULL, NULL, &maMixSound);

        if (result != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to load audio file");
        }

        ma_uint64 lengthInFrames;
        if (ma_sound_get_length_in_pcm_frames(&maMixSound, &lengthInFrames) != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to get sound length");
        }

        duration = (double)lengthInFrames / sampleRate;
        ma_sound_set_end_callback(&maMixSound, onAudioEnd, &tsfnCallback);
    }
    ~MixSound()
    {
        ma_sound_uninit(&maMixSound);
        tsfnCallback.Release();
        std::cout << "Sound destroyed\n";
    }
    float getDuration()
    {
        return duration;
    }
    void play()
    {
        ma_sound_start(&maMixSound);
    }
    void pause()
    {
        ma_sound_stop(&maMixSound);
    }
    void seek(float position)
    {
        ma_sound_seek_to_pcm_frame(&maMixSound, (ma_uint64)(position * sampleRate));
    }
    float getTrackPosition()
    {
        ma_uint64 position;
        ma_sound_get_cursor_in_pcm_frames(&maMixSound, &position);
        return (double)position / sampleRate;
    }
    void setVolume(float volume)
    {
        ma_sound_set_volume(&maMixSound, ma_volume_db_to_linear(volume));
    }
    float getVolume()
    {
        return ma_volume_linear_to_db(ma_sound_get_volume(&maMixSound));
    }
    void setLooping(bool loop)
    {
        ma_sound_set_looping(&maMixSound, loop);
    }
    void setPitch(float pitch)
    {
        ma_sound_set_pitch(&maMixSound, pitch);
    }
    void setPan(float pan)
    {
        ma_sound_set_pan(&maMixSound, pan);
    }
    float getPan()
    {
        return ma_sound_get_pan(&maMixSound);
    }
    float getPitch()
    {
        return ma_sound_get_pitch(&maMixSound);
    }
    void setAudioEndCallback(Napi::ThreadSafeFunction callback)
    {
        tsfnCallback = callback;
    }
};

std::unordered_map<int, std::unique_ptr<MixSound>> soundMap;
unsigned int numSounds = 0;

// Napi::ThreadSafeFunction tsfn;

// Expected arguments - audioFilePath: string
// Returns: soundId - integer OR -1 if failed
Napi::Number createNewSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
        Napi::TypeError::New(env, "Expected 2 arguments").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Expected a string as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    std::string audioFilePath = info[0].As<Napi::String>().Utf8Value();

    auto newSound = std::make_unique<MixSound>(audioFilePath);
    numSounds++;

    soundMap[numSounds] = std::move(newSound);

    return Napi::Number::New(env, numSounds);
}

Napi::Boolean setAudioEndCallback(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 2 && !info[0].IsNumber() && !info[1].IsFunction())
    {
        Napi::TypeError::New(env, "Expected a number as first argument and a function as second argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(env, info[1].As<Napi::Function>(), "TSFN", 0, 3);

    tsfn.Unref(env);
    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap[soundId]->setAudioEndCallback(tsfn);

    return Napi::Boolean::New(env, true);
}

Napi::Boolean destroySound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap.erase(soundId);

    return Napi::Boolean::New(env, true);
}

// Expected arguments - soundId: integer
// Returns - success: boolean
Napi::Boolean playSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap[soundId]->play();

    return Napi::Boolean::New(env, true);
}
Napi::Boolean pauseSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap[soundId]->pause();

    return Napi::Boolean::New(env, true);
}

Napi::Boolean seekSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 2 && !info[0].IsNumber() && !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();
    float position = info[1].As<Napi::Number>().FloatValue();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap[soundId]->seek(position);

    return Napi::Boolean::New(env, true);
}

Napi::Number getSoundPosition(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    return Napi::Number::New(env, soundMap[soundId]->getTrackPosition());
}

Napi::Number getSoundDuration(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    return Napi::Number::New(env, soundMap[soundId]->getDuration());
}

Napi::Boolean setSoundVolume(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 2 && !info[0].IsNumber() && !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();
    float volume = info[1].As<Napi::Number>().FloatValue();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap[soundId]->setVolume(volume);

    return Napi::Boolean::New(env, true);
}

Napi::Number getSoundVolume(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    return Napi::Number::New(env, soundMap[soundId]->getVolume());
}

Napi::Boolean setSoundPitch(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 2 && !info[0].IsNumber() && !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();
    float pitch = info[1].As<Napi::Number>().FloatValue();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap[soundId]->setPitch(pitch);

    return Napi::Boolean::New(env, true);
}

Napi::Number getSoundPitch(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    return Napi::Number::New(env, soundMap[soundId]->getPitch());
}

Napi::Boolean setSoundPan(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 2 && !info[0].IsNumber() && !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();
    float pan = info[1].As<Napi::Number>().FloatValue();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap[soundId]->setPan(pan);

    return Napi::Boolean::New(env, true);
}

Napi::Number getSoundPan(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    return Napi::Number::New(env, soundMap[soundId]->getPan());
}

Napi::Boolean setSoundLooping(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 2 && !info[0].IsNumber() && !info[1].IsBoolean())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int soundId = info[0].As<Napi::Number>().Int32Value();
    bool loop = info[1].As<Napi::Boolean>().Value();

    if (soundMap.find(soundId) == soundMap.end())
    {
        Napi::TypeError::New(env, "Sound not found").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    soundMap[soundId]->setLooping(loop);

    return Napi::Boolean::New(env, true);
}

Napi::Boolean initAudioEngine(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    ma_result result = ma_engine_init(NULL, &engine);

    if (result != MA_SUCCESS)
    {

        return Napi::Boolean::New(env, false);
    }
    sampleRate = ma_engine_get_sample_rate(&engine);

    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS)
    {
        return Napi::Boolean::New(env, false);
    }
    if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackDeviceCount, NULL, NULL) != MA_SUCCESS)
    {
        std::cout << "Failed to get playback devices\n";
        return Napi::Boolean::New(env, false);
    }
    return Napi::Boolean::New(env, true);
}

Napi::Array getPlaybackDevices(const Napi::CallbackInfo &info)
{

    Napi::Array devices = Napi::Array::New(info.Env(), playbackDeviceCount);
    for (ma_uint32 iDevice = 0; iDevice < playbackDeviceCount; iDevice += 1)
    {
        Napi::Object device = Napi::Object::New(info.Env());
        device.Set("id", Napi::Number::New(info.Env(), iDevice));
        device.Set("name", Napi::String::New(info.Env(), pPlaybackInfos[iDevice].name));
        device.Set("isDefault", Napi::Boolean::New(info.Env(), pPlaybackInfos[iDevice].isDefault));

        devices.Set(iDevice, device);
    }
    return devices;
}

Napi::Boolean setPlaybackDevice(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1 && !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    int deviceId = info[0].As<Napi::Number>().Int32Value();

    ma_engine_config config = ma_engine_config_init();

    config.pPlaybackDeviceID = &pPlaybackInfos[deviceId].id;

    if (ma_engine_init(&config, &engine) != MA_SUCCESS)
    {
        return Napi::Boolean::New(env, false);
    }

    return Napi::Boolean::New(env, true);
}

Napi::Object EntryPoint(Napi::Env env, Napi::Object exports)
{

    exports.Set(Napi::String::New(env, "initAudioEngine"), Napi::Function::New(env, initAudioEngine));
    exports.Set(Napi::String::New(env, "setPlaybackDevice"), Napi::Function::New(env, setPlaybackDevice));
    exports.Set(Napi::String::New(env, "createNewSound"), Napi::Function::New(env, createNewSound));
    exports.Set(Napi::String::New(env, "setAudioEndCallback"), Napi::Function::New(env, setAudioEndCallback));
    exports.Set(Napi::String::New(env, "destroySound"), Napi::Function::New(env, destroySound));
    exports.Set(Napi::String::New(env, "playSound"), Napi::Function::New(env, playSound));
    exports.Set(Napi::String::New(env, "pauseSound"), Napi::Function::New(env, pauseSound));
    exports.Set(Napi::String::New(env, "getSoundDuration"), Napi::Function::New(env, getSoundDuration));
    exports.Set(Napi::String::New(env, "getSoundPosition"), Napi::Function::New(env, getSoundPosition));
    exports.Set(Napi::String::New(env, "setSoundVolume"), Napi::Function::New(env, setSoundVolume));
    exports.Set(Napi::String::New(env, "getSoundVolume"), Napi::Function::New(env, getSoundVolume));
    exports.Set(Napi::String::New(env, "setSoundPitch"), Napi::Function::New(env, setSoundPitch));
    exports.Set(Napi::String::New(env, "getSoundPitch"), Napi::Function::New(env, getSoundPitch));
    exports.Set(Napi::String::New(env, "setSoundPan"), Napi::Function::New(env, setSoundPan));
    exports.Set(Napi::String::New(env, "getSoundPan"), Napi::Function::New(env, getSoundPan));
    exports.Set(Napi::String::New(env, "setSoundLooping"), Napi::Function::New(env, setSoundLooping));
    exports.Set(Napi::String::New(env, "seekSound"), Napi::Function::New(env, seekSound));
    exports.Set(Napi::String::New(env, "getPlaybackDevices"), Napi::Function::New(env, getPlaybackDevices));

    // exports.Set(Napi::String::New(env, "setFadeInPeriod"), Napi::Function::New(env, setFadeInPeriod));
    // exports.Set(Napi::String::New(env, "onAudioEnd"), Napi::Function::New(env, onAudioEnd));
    // exports.Set(Napi::String::New(env, "loadAudioFile"), Napi::Function::New(env, loadAudioFile));
    // exports.Set(Napi::String::New(env, "isAudioPlaying"), Napi::Function::New(env, isPlaying));
    // exports.Set(Napi::String::New(env, "playAudio"), Napi::Function::New(env, playAudio));
    // exports.Set(Napi::String::New(env, "pauseAudio"), Napi::Function::New(env, pauseAudio));
    // exports.Set(Napi::String::New(env, "seekAudio"), Napi::Function::New(env, seekAudio));
    // exports.Set(Napi::String::New(env, "rewindAudio"), Napi::Function::New(env, RewindAudio));
    // exports.Set(Napi::String::New(env, "getAudioDevices"), Napi::Function::New(env, getAudioDevices));
    // exports.Set(Napi::String::New(env, "setAudioDevice"), Napi::Function::New(env, setAudioDevice));
    // exports.Set(Napi::String::New(env, "setVolume"), Napi::Function::New(env, setVolume));
    // exports.Set(Napi::String::New(env, "getVolume"), Napi::Function::New(env, getVolume));
    // exports.Set(Napi::String::New(env, "getAudioDuration"), Napi::Function::New(env, getDuration));
    // exports.Set(Napi::String::New(env, "getAudioPosition"), Napi::Function::New(env, getAudioPosition));
    // exports.Set(Napi::String::New(env, "destroySDL"), Napi::Function::New(env, destroy));

    return exports;
}

NODE_API_MODULE(addon, EntryPoint);