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
        if (music == nullptr) {
            Mix_FreeMusic(music);
        }
    }

public:
    MusicPlayer() {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            Log::error("Failed to init SDL audio");
        }
        int flags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG;
        if (Mix_Init(flags) != flags) {
            Log::error("Failed to initialize Mixer: %s", Mix_GetError());
        }
        Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024);
        mixInit = true;
    }

    ~MusicPlayer() {
        freeMusic();
        SDL_Quit();
    }

    void play(std::string song) {
        if (!mixInit) {
            Log::error("SDL Mixer not initialized!");
            return;
        }
        freeMusic();
        music = Mix_LoadMUS(song.c_str());
        Mix_PlayMusic(music, 1);
    }
};

#endif
