#pragma once

typedef struct _Mix_Music Mix_Music;

namespace MusicPlayer
{
	void Play(Mix_Music* music);
	void PlayWithIntro(Mix_Music* music, Mix_Music* intro);

	bool IsPlaying();

	void Pause();
	void Resume();

	void Stop();

	void SetVolume(float volume); // from 0 to 100
	float Volume(); // from 0 to 100

	//TODO: Wrap Mix_FadeInMusic() and Mix_FadeOutMusic()
}
