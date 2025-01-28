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
    Napi::ThreadSafeFunction tsfnCallback;

    static void onAudioEnd(void *passed, ma_sound *pSound)
    {

        Napi::ThreadSafeFunction tsfn = *(Napi::ThreadSafeFunction *)passed;
        tsfn.NonBlockingCall();
    }

public:
    MixSound(std::string audioPath, Napi::ThreadSafeFunction callback)
    {
        audioFilePath = audioPath;
        tsfnCallback = callback;

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
};

std::unordered_map<int, std::unique_ptr<MixSound>> soundMap;

// Napi::ThreadSafeFunction tsfn;

// Expected arguments - audioFilePath: string, id: integer
// Returns: soundId - integer OR -1 if failed
Napi::Number createNewSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 3)
    {
        Napi::TypeError::New(env, "Expected 2 arguments").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Expected a string as first argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number as second argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }
    if (!info[2].IsFunction())
    {
        Napi::TypeError::New(env, "Expected a function as third argument").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    std::string audioFilePath = info[0].As<Napi::String>().Utf8Value();
    int id = info[1].As<Napi::Number>().Int32Value();
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(env, info[2].As<Napi::Function>(), "TSFN", 0, 3);

    auto newSound = std::make_unique<MixSound>(audioFilePath, tsfn);

    soundMap[id] = std::move(newSound);

    soundMap[id]->play();

    return Napi::Number::New(env, 1);
}

// Expected arguments - soundId: integer
// Returns - success: boolean
Napi::Boolean playSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    return Napi::Boolean::New(env, true);
}
Napi::Boolean pauseSound(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    return Napi::Boolean::New(env, true);
}

Napi::Number getSoundDuration(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    return Napi::Number::New(env, 0);
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