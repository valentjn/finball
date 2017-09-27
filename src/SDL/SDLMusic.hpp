#ifndef SDL_SDLMUSIC_HPP_
#define SDL_SDLMUSIC_HPP_

#include <SDL2/SDL_mixer.h>
#include <string>

#include "Log.hpp"

class SDLMusic{
private:
	bool initialized;
	Mix_Music *music;

public:
	SDLMusic() : music(nullptr) {
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) { // 1024
			Log::error("Failed to initialize SDLMusic: %s", Mix_GetError());
			initialized = false;
		} else {
			initialized = true;
		}
	}

	~SDLMusic(){
		Mix_FreeMusic(music);
		Mix_CloseAudio();
	}

	void play(std::string file, int repetitions=-1) {
		if (!initialized) {
			Log::error("SDLMusic not initialized. Can not play music!");
			return;
		}
		if (music != nullptr) {
			Mix_FreeMusic(music);
		}
		music = Mix_LoadMUS(file.c_str());
		if (music == nullptr) {
			Log::error("Could not load music %s: %s", file, Mix_GetError());
		}
		else if (Mix_PlayMusic(music, repetitions) == -1) {
			Log::error("Could not play music %s: %s", file, Mix_GetError());
		}
	}
};


#endif /* SDL_SDLMUSIC_HPP_ */
