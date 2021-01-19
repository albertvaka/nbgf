#include "musicplayer.h"

#include <SDL_mixer.h>

namespace MusicPlayer
{
	Mix_Music* toPlayAfterIntro;
	void introFinishedHook() { Mix_PlayMusic(toPlayAfterIntro, -1); }

	void Play(Mix_Music* music) {
		Mix_PlayMusic(music, -1);
	}
	
	void PlayWithIntro(Mix_Music* music, Mix_Music* intro) {
		Mix_PlayMusic(intro, 1);
		toPlayAfterIntro = music;
		Mix_HookMusicFinished(introFinishedHook);
	}

	bool IsPlaying() {
		return Mix_PlayingMusic();
	}

	void Pause() {
		Mix_PauseMusic();
	}

	void Resume() {
		Mix_ResumeMusic();
	}

	void Stop() {
		Mix_HaltMusic();
	}

	void SetVolume(float volume) {
		Mix_VolumeMusic(volume * (128 / 100.f));
	}
	
	float GetVolume(float volume) {
		return Mix_VolumeMusic(-1) * (100 / 128.f);
	}
}