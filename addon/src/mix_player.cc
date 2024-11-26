
#define MINIAUDIO_IMPLEMENTATION
#define NAPI_VERSION 8

#include "../node-addon-api/napi.h"

#include <iostream>
#include <string>
#include <thread>

#include "./miniaudio.h"

using namespace Napi;
using std::cout;

ma_engine engine;
ma_sound audio;

Napi::ThreadSafeFunction tsfn;

ma_engine_config config;

void destroy(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    ma_engine_uninit(&engine);
}

void getPlaybackDevices(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
}

void pauseAudio(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    ma_sound_stop(&audio);
}

void playAudio(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    ma_sound_start(&audio);
}

void seekAudio(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
        napi_throw_error(env, 0, "incorrect number of arguments passed");
    }

    if (!info[0].IsNumber())
    {
        napi_throw_error(env, 0, "incorrect argument type, expected number");
    }

    int time = info[0].As<Napi::Number>().Int32Value();

    ma_sound_seek_to_pcm_frame(&audio, time * config.sampleRate);
}

void loadAudioFile(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
        napi_throw_error(env, 0, "incorrect number of arguments passed");
    }

    if (!info[0].IsString())
    {
        napi_throw_error(env, 0, "incorrect argument type, expected string");
    }

    std::string filePath = info[0].As<Napi::String>().Utf8Value();

    if (ma_sound_init_from_file(&engine, filePath.c_str(), 0, NULL, NULL, &audio) != MA_SUCCESS)
    {
        napi_throw_error(env, 0, "Could not load given audio file");
    }

    ma_sound_set_end_callback(&audio, [](void *pUserData, ma_sound *pSound)
                              { tsfn.NonBlockingCall(); }, env);

    return;
}

void onAudioEnd(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
        napi_throw_error(env, 0, "incorrect number of arguments passed");
    }

    if (!info[0].IsFunction())
    {
        napi_throw_error(env, 0, "incorrect argument type, expected function");
    }
    tsfn = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "TSFN", 0, 3);
}

Napi::Boolean isPlaying(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    return Napi::Boolean::New(env, ma_sound_is_playing(&audio));
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{

    ma_result result;
    ma_context context;
    ma_device_info *pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    ma_device_info *pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;
    ma_uint32 iDevice;

    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS)
    {
        napi_throw_error(env, 0, "Could not initialize audio engine");
    }

    result = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);

    printf("Playback Devices\n");
    for (iDevice = 0; iDevice < playbackDeviceCount; ++iDevice)
    {
        printf("    %u: %s\n", iDevice, pPlaybackDeviceInfos[iDevice].name);
    }

    config = ma_engine_config_init();

    config.sampleRate = 48000;

    result = ma_engine_init(&config, &engine);

    if (result != MA_SUCCESS)
    {
        napi_throw_error(env, 0, "Could not initialize audio engine");
    }

    exports.Set(Napi::String::New(env, "loadAudioFile"), Napi::Function::New(env, loadAudioFile));
    exports.Set(Napi::String::New(env, "destroy"), Napi::Function::New(env, destroy));
    exports.Set(Napi::String::New(env, "playAudio"), Napi::Function::New(env, playAudio));
    exports.Set(Napi::String::New(env, "pauseAudio"), Napi::Function::New(env, pauseAudio));
    exports.Set(Napi::String::New(env, "seekAudio"), Napi::Function::New(env, seekAudio));
    exports.Set(Napi::String::New(env, "onAudioEnd"), Napi::Function::New(env, onAudioEnd));
    exports.Set(Napi::String::New(env, "isPlaying"), Napi::Function::New(env, isPlaying));

    return exports;
}

NODE_API_MODULE(addon, Init)
