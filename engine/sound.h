#pragma once

#include <array>
#include "rand.h"

struct Mix_Chunk;

struct Sound
{
	void Load(const char* path);
	~Sound();

	int Play() const; //returns a channel id
	static bool Playing(int channel_id);
	static void Stop(int channel_id);

	void SetVolume(float v); //from 0 to 100
	float Volume(); //from 0 to 100

private:
	Mix_Chunk* sound = nullptr;
};

template<size_t Size>
struct MultiSound
{
	void Load(std::array<const char*, Size> paths) 
	{
		for (int i = 0; i < Size; i++) {
			sounds[i].Load(paths[i]);
		}
	}

	int Play() {
		return sounds[Rand::roll(Size)].Play();
	}

private:
	std::array<Sound, Size> sounds;
};
