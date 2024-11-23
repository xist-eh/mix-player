
#include <iostream>

#include "./lib/include/SDL.h"
#include "./lib/include/SDL_mixer.h"

#include "node.h"

using namespace v8;

Mix_Music *music;

const char *ToCString(const String::Utf8Value &value)
{
    return *value ? *value : "<string conversion failed>";
}

void playAudioFile(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    // Expected argument: path of audio file to play (string)

    if (Mix_PlayingMusic() != 0)
    {
        return;
    }

    Isolate *isolate = args.GetIsolate();

    // Check the number of arguments passed.
    if (args.Length() != 1)
    {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments")
                .ToLocalChecked()));
        return;
    }
    // Type check argument
    if (!args[0]->IsString())
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong arguments")
                .ToLocalChecked()));
        return;
    }

    // Convert argument from v8 string to cstring
    String::Utf8Value str(isolate, args[0]);

    const char *audioFilePath = ToCString(str);

    // Check if valid path
    struct stat sb;
    if (stat(audioFilePath, &sb) != 0)
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Path doesn't exist")
                .ToLocalChecked()));
        return;
    }

    Mix_Music *temp = Mix_LoadMUS(audioFilePath);

    music = temp;
    Mix_PlayMusic(music, 1);
}

void pauseAudio(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    Mix_PauseMusic();
}

void playAudio(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    Mix_ResumeMusic();
}

void isPlaying(const v8::FunctionCallbackInfo<v8::Value> &args)
{

    args.GetReturnValue().Set(Mix_PausedMusic() == 0 ? true : false);
}

void getAudioDuration(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    args.GetReturnValue().Set(Number::New(isolate, Mix_MusicDuration(music)));
}

void hasAudioEnded(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    args.GetReturnValue().Set((Mix_GetMusicPosition(music) >= Mix_MusicDuration(music)) ? true : false);
}

void setAudioPosition(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    if (args.Length() != 1)
    {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments")
                .ToLocalChecked()));
        return;
    }
    // Type check argument
    if (!args[0]->IsNumber())
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong arguments")
                .ToLocalChecked()));
        return;
    }

    double pos = args[0].As<Number>()->Value();

    Mix_SetMusicPosition(pos);
}

void Initialize(v8::Local<v8::Object> exports)
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

    NODE_SET_METHOD(exports, "playAudio", playAudio);
    NODE_SET_METHOD(exports, "isPlaying", isPlaying);
    NODE_SET_METHOD(exports, "playAudioFile", playAudioFile);
    NODE_SET_METHOD(exports, "pauseAudio", pauseAudio);
    NODE_SET_METHOD(exports, "hasAudioEnded", hasAudioEnded);
    NODE_SET_METHOD(exports, "setAudioPosition", setAudioPosition);
    NODE_SET_METHOD(exports, "getAudioDuration", getAudioDuration);
}

NODE_MODULE(addon, Initialize)