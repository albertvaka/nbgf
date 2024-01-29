#include "musicplayer.h"

#include "raw_input.h"

#include <SDL_mixer.h>
#include <SDL_assert.h>

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

	void Pause(bool keepPausedOnChange) {
		stayPaused = keepPausedOnChange;
		Mix_PauseMusic();
	}

	bool IsPaused() {
		return Mix_PausedMusic();
	}

	void Resume() {
		stayPaused = false;
		Mix_ResumeMusic();
	}

	void Stop() {
		Mix_HaltMusic();
		current = nullptr;
	}

	void SetVolume(float v) {
		SDL_assert(v >= 0.f && v <= 1.f);
		Mix_VolumeMusic(v * 128);
	}
	
	float Volume() {
		return Mix_VolumeMusic(-1) / 128.f;
	}

	void ToggleMusicWithM(bool keepPausedOnChange) {
		if (Keyboard::IsKeyJustPressed(SDL_SCANCODE_M)) {
			if (IsPaused()) {
				Resume();
			} else {
				Pause(keepPausedOnChange);
			}
		}
	}
}
