#ifndef MUSIC_PLAYER_HPP_
#define MUSIC_PLAYER_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <string>

#include "Log.hpp"

class MusicPlayer {
private:
    Mix_Music *music = nullptr;
    bool mixInit = false;

    void freeMusic() {
        if (music != nullptr) {
            Mix_FreeMusic(music);
        }
    }

public:
    MusicPlayer() {
        int flags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG;
        if (Mix_Init(flags) != flags) {
            Log::error("Failed to initialize SDL Mixer: %s", Mix_GetError());
            return;
        }
        Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024);
        mixInit = true;

        Log::debug("Initialized SDL mixer.");
    }

    ~MusicPlayer() {
        freeMusic();
    }

    void play(std::string musicFilePath) {
        play(musicFilePath.c_str());
    }

    void play(const char *musicFilePath) {
        Log::info("Playing music: %s", musicFilePath);
        if (!mixInit) {
            Log::error("SDL Mixer not initialized!");
            return;
        }
        freeMusic();
        music = Mix_LoadMUS(musicFilePath);
        Mix_PlayMusic(music, 1);
    }
};

#endif
