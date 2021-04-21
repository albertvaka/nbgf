#include "musicplayer.h"

#include <SDL_mixer.h>

#include "raw_input.h"

namespace MusicPlayer
{
	Mix_Music* toPlayAfterIntro;
	Mix_Music* current;
	bool stayPaused;
	void introFinishedHook() { Mix_PlayMusic(toPlayAfterIntro, -1); }

	void Play(Mix_Music* music) {
		Mix_PlayMusic(music, -1);
		current = music;
		if (stayPaused) {
			Pause();
		}
	}
	
	void PlayWithIntro(Mix_Music* music, Mix_Music* intro) {
		Mix_PlayMusic(intro, 1);
		toPlayAfterIntro = music;
		current = music;
		Mix_HookMusicFinished(introFinishedHook);
		if (stayPaused) {
			Pause();
		}
	}

	bool IsPlaying() {
		return Mix_PlayingMusic();
	}

	Mix_Music* GetPlaying() {
		return current; 
	}

	void Pause() {
		Mix_PauseMusic();
	}

	bool IsPaused() {
		return Mix_PausedMusic();
	}

	void Resume() {
		Mix_ResumeMusic();
	}

	void Stop() {
		Mix_HaltMusic();
		current = nullptr;
	}

	void SetVolume(float volume) {
		Mix_VolumeMusic(volume * (128 / 100.f));
	}
	
	float Volume() {
		return Mix_VolumeMusic(-1) * (100 / 128.f);
	}

	void ToggleMusicWithM(bool keepPausedOnChange) {
		if (Keyboard::IsKeyJustPressed(SDL_SCANCODE_M)) {
			if (IsPaused()) {
				Resume();
				stayPaused = false;
			} else {
				Pause();
				stayPaused = keepPausedOnChange;
			}
		}
	}
}