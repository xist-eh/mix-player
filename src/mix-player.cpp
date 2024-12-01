#define NAPI_VERSION 8

#include "../node_modules/node-addon-api/napi.h"

#include <string>

#include "SDL.h"
#include "SDL_mixer.h"

Mix_Music *audioTrack = nullptr;

Napi::ThreadSafeFunction tsfn;

void onAudioEnd(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1)
    {
        napi_throw_error(env, 0, "Incorrect number of arguments passed!");
    }
    if (!info[0].IsFunction())
    {
        napi_throw_error(env, 0, "onAudioEnd: expected function");
    }

    tsfn = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "TSFN", 0, 3);

    Mix_HookMusicFinished([]()
                          { tsfn.NonBlockingCall(); });
}

Napi::Array getAudioDevices(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    int numDevices = SDL_GetNumAudioDevices(0);
    Napi::Array devices = Napi::Array::New(env, numDevices);
    for (int i = 0; i < numDevices; i++)
    {
        devices[i] = Napi::String::New(env, SDL_GetAudioDeviceName(i, 0));
    }
    return devices;
}

void setAudioDevice(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
        napi_throw_error(env, 0, "Incorrect number of arguments passed to setAudioDevice()");
        return;
    }
    if (!info[0].IsNumber())
    {
        napi_throw_error(env, 0, "Expected type number to be passed to setAudioDevice()");
        return;
    }

    const int index = info[0].As<Napi::Number>().Int32Value();
    if (index > SDL_GetNumAudioDevices(0) - 1)
    {
        napi_throw_error(env, 0, "Index supplied to setAudioDevice exceeds number of audio devices!");
    }
    Mix_CloseAudio();
    Mix_OpenAudioDevice(48000, AUDIO_F32, 2, 2048, SDL_GetAudioDeviceName(index, 0), SDL_AUDIO_ALLOW_FORMAT_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
}

Napi::Boolean loadAudioFile(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    bool success = false;

    if (info.Length() != 1)
    {
        napi_throw_error(env, 0, "Incorrect number of arguments passed to playAudio!");
    }
    if (!info[0].IsString() && !info[0].IsNull())
    {
        napi_throw_error(env, 0, "playAudio(): expected string or null as argument");
    }

    if (!info[0].IsNull())
    {
        audioTrack = Mix_LoadMUS(info[0].As<Napi::String>().Utf8Value().c_str());
    }
    else
    {
        Mix_RewindMusic();
    }

    if (Mix_PlayMusic(audioTrack, 0) == 0)
    {
        success = true;
    }

    return Napi::Boolean::New(env, success);
}

Napi::Boolean isPlaying(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    return Napi::Boolean::New(env, Mix_PlayingMusic() == 0 ? false : true);
}

Napi::Boolean playAudio(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    bool success = false;

    if (audioTrack == nullptr || Mix_PlayingMusic() != 0)
    {
        return Napi::Boolean::New(env, success);
    }

    Mix_ResumeMusic();

    return Napi::Boolean::New(env, success);
}

void pauseAudio(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    Mix_PauseMusic();
}

void seekAudio(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
        napi_throw_error(env, 0, "Incorrect number of arguments passed to playAudio!");
    }
    if (!info[0].IsNumber())
    {
        napi_throw_error(env, 0, "seekAudio(): expected number as argument");
    }

    double pos = info[0].As<Napi::Number>().DoubleValue();

    Mix_SetMusicPosition(pos);
}

void setVolume(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
        napi_throw_error(env, 0, "Incorrect number of arguments passed to setVolume!");
    }
    if (!info[0].IsNumber())
    {
        napi_throw_error(env, 0, "setVolume(): expected number as argument");
    }

    int vol = info[0].As<Napi::Number>().Int32Value();

    Mix_VolumeMusic(vol);
}

Napi::Number getDuration(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    return Napi::Number::New(env, Mix_MusicDuration(NULL));
}

Napi::Object EntryPoint(Napi::Env env, Napi::Object exports)
{
    int result = 0;
    int flags = MIX_INIT_MP3;

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("Failed to init SDL\n");
        exit(1);
    }

    if (flags != (result = Mix_Init(flags)))
    {
        printf("Could not initialize mixer (result: %d).\n", result);
        printf("Mix_Init: %s\n", Mix_GetError());
        exit(1);
    }

    Mix_OpenAudio(48000, AUDIO_F32, 2, 2048);

    exports.Set(Napi::String::New(env, "onAudioEnd"), Napi::Function::New(env, onAudioEnd));
    exports.Set(Napi::String::New(env, "loadAudioFile"), Napi::Function::New(env, loadAudioFile));
    exports.Set(Napi::String::New(env, "isAudioPlaying"), Napi::Function::New(env, isPlaying));
    exports.Set(Napi::String::New(env, "playAudio"), Napi::Function::New(env, playAudio));
    exports.Set(Napi::String::New(env, "pauseAudio"), Napi::Function::New(env, pauseAudio));
    exports.Set(Napi::String::New(env, "seekAudio"), Napi::Function::New(env, seekAudio));
    exports.Set(Napi::String::New(env, "getAudioDevices"), Napi::Function::New(env, getAudioDevices));
    exports.Set(Napi::String::New(env, "setAudioDevice"), Napi::Function::New(env, setAudioDevice));
    exports.Set(Napi::String::New(env, "setVolume"), Napi::Function::New(env, setVolume));
    exports.Set(Napi::String::New(env, "getAudioDuration"), Napi::Function::New(env, getDuration));

    return exports;
}

NODE_API_MODULE(addon, EntryPoint);