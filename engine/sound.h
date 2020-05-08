#pragma once

struct Mix_Chunk;
typedef struct _Mix_Music Mix_Music;

struct Sound
{
	void load(const char* path);
	~Sound();

	void setVolume(float v); //from 0 to 100
	void play();

private:
	Mix_Chunk* sound = nullptr;
};

namespace MusicPlayer
{
	void Play(Mix_Music* music);
	void PlayWithIntro(Mix_Music* music, Mix_Music* intro);

	bool IsPlaying();

	void Pause();
	void Resume();

	void Stop();

	void SetVolume(float volume); // from 0 to 100
	float GetVolume(float volume); // from 0 to 100

	//TODO: Wrap Mix_FadeInMusic() and Mix_FadeOutMusic()
}
