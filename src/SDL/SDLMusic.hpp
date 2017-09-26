/*
 * SDLMusic.hpp
 *
 *  Created on: Sep 26, 2017
 *      Author: Steffen Seckler
 */
#ifndef SRC_SDL_SDLMUSIC_HPP_
#define SRC_SDL_SDLMUSIC_HPP_
#include <SDL2/SDL_mixer.h>

class SDLMusic{
private:
	bool initialized;
	Mix_Music *music;
public:
	SDLMusic() :
			music(nullptr) {
		if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
			Log::warn("could not initialize audio play back");
			initialized = false;
		} else {
			initialized = true;
		}
	}

	~SDLMusic(){
		Mix_FreeMusic( music );
		Mix_CloseAudio();
	}
	void load(std::string file, int repetitions=-1){
		if(!initialized){
			Log::warn("Music not initialized, can not load");
		}
		if(music != nullptr){
			Mix_FreeMusic(music);
		}
		music = Mix_LoadMUS(file.c_str());
		if (music==nullptr){
			Log::warn("could not load music %s", file);
		}
		if (Mix_PlayMusic(music, repetitions) == -1) {
			Log::warn("could not start music");
		}
	}
};


#endif /* SRC_SDL_SDLMUSIC_HPP_ */
