#pragma once

#ifndef _USE_MATH_DEFINES 
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <SDL_mixer.h>

namespace MusicPlayer
{
	void Play(Mix_Music* music);
	void PlayWithIntro(Mix_Music* music, Mix_Music* intro);

	bool IsPlaying();
	Mix_Music* GetPlaying();

	inline void Ensure(Mix_Music* music) { // Play if not playing
		if (GetPlaying() != music) {
			Play(music);
		}
	}

	inline void EnsureWithIntro(Mix_Music* music, Mix_Music* intro) {
		if (GetPlaying() != music) {
			PlayWithIntro(music, intro);
		}
	}

	void Pause(bool keepPausedOnMusicChange = false);
	bool IsPaused();
	void Resume();

	void Stop();

	void SetVolume(float volume); // from 0 to 100
	float Volume(); // from 0 to 100

	void ToggleMusicWithM(bool keepPausedOnMusicChange = true);

	//TODO: Wrap Mix_FadeInMusic() and Mix_FadeOutMusic()
}
