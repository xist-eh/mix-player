#define NAPI_VERSION 4

#define NODE_ADDON_API_CPP_EXCEPTIONS 1
#define MA_NO_ENCODING

#define MINIAUDIO_IMPLEMENTATION

#include "../node_modules/node-addon-api/napi.h"

#include "miniaudio.h"

#include <string>
#include <vector>
#include <iostream>

ma_engine engine;
// ma_engine_get_sample_rate(&engine)
std::vector<std::unique_ptr<ma_sound>> sounds;
ma_uint32 sampleRate;

ma_context context;
ma_device_info *pPlaybackInfos;
ma_uint32 playbackDeviceCount;

// Napi::ThreadSafeFunction tsfn;

// Expected arguments - audioFilePath: string
// Returns: soundId - integer OR -1 if failed
Napi::Number createNewSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Expected a string as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    auto sound = std::make_unique<ma_sound>();

    ma_result result = ma_sound_init_from_file(&engine, info[0].As<Napi::String>().ToString().Utf8Value().c_str(), 0, NULL, NULL, sound.get());
    if (result != MA_SUCCESS)
    {
        return Napi::Number::New(env, -1);
    }

    sounds.push_back(std::move(sound));

    return Napi::Number::New(env, sounds.size() - 1);
}

// Expected arguments - soundId: integer
// Returns - success: boolean
Napi::Boolean playSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    ma_sound_start(sounds.at(info[0].ToNumber().Int32Value()).get());

    return Napi::Boolean::New(env, true);
}
Napi::Boolean pauseSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    ma_sound_stop(sounds.at(info[0].ToNumber().Int32Value()).get());

    return Napi::Boolean::New(env, true);
}

Napi::Number getSoundDuration(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    int32_t soundIndex = info[0].ToNumber().Int32Value();
    ma_sound *sound = sounds.at(soundIndex).get();

    ma_uint64 lengthInFrames;
    ma_result result = ma_sound_get_length_in_pcm_frames(sound, &lengthInFrames);
    if (result != MA_SUCCESS)
    {
        Napi::Error::New(env, "Failed to get sound length").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    double durationInSeconds = (double)lengthInFrames / sampleRate;

    return Napi::Number::New(env, durationInSeconds);
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
    return Napi::Boolean::New(env, true);
}

Napi::Array getPlaybackDevices(const Napi::CallbackInfo &info)
{
    if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackDeviceCount, NULL, NULL) != MA_SUCCESS)
    {
        std::cout << "Failed to get playback devices\n";
        return Napi::Array::New(info.Env());
    }
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

Napi::Object EntryPoint(Napi::Env env, Napi::Object exports)
{

    exports.Set(Napi::String::New(env, "initAudioEngine"), Napi::Function::New(env, initAudioEngine));
    exports.Set(Napi::String::New(env, "createNewSound"), Napi::Function::New(env, createNewSound));
    exports.Set(Napi::String::New(env, "playSound"), Napi::Function::New(env, playSound));
    exports.Set(Napi::String::New(env, "pauseSound"), Napi::Function::New(env, pauseSound));
    exports.Set(Napi::String::New(env, "getSoundDuration"), Napi::Function::New(env, getSoundDuration));
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