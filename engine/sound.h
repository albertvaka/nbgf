#pragma once

#include <array>
#include "rand.h"
#include "vec.h"

struct Mix_Chunk;

// Note: By default, SDL_Mixer can only play 8 sounds at once

struct Sound
{
	void Load(const char* path);
	~Sound();

	int Play() const; //returns a channel id
	int Play(vec source, vec listener, float silenceDistance = 300.f) const; //returns a channel id
	int PlayInLoop() const; // plays forever until stopped, returns a channel id

	static bool Playing(int channel_id);
	static void Stop(int channel_id);

	void SetVolume(float v); //from 0 to 100
	float Volume() const; //from 0 to 100

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

	int Play(vec source, vec listener, float silenceDistance = 300.f) const {
		return sounds[Rand::roll(Size)].Play(source, listener, silenceDistance);
	}

	const Sound& operator[](int i) {
		return sounds[i];
	}

private:
	std::array<Sound, Size> sounds;
};
