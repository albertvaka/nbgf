#pragma once

#include <array>
#include "rand.h"
#include "vec.h"

#include <SDL_mixer.h>

// Note: By default, SDL_Mixer can only play 8 sounds at once

struct Sound
{
	void Load(const char* path);
	~Sound();

	int Play() const; // returns a channel id
	int Play(vec source, vec listener, float silenceDistance = 300.f) const; // returns a channel id
	int PlayInLoop() const; // plays forever until stopped, returns a channel id

	static bool Playing(int channel_id);
	static void Stop(int channel_id);

	void SetVolume(float volume); // between 0 and 1
	float Volume() const; // between 0 and 1

private:
	Mix_Chunk* sound = nullptr;
};

template<size_t Size>
struct MultiSound
{
	template<std::size_t N>
	void Load(const char * const (&paths)[N]) {
		static_assert(N == Size);
		for (int i = 0; i < Size; i++) {
			sounds[i].Load(paths[i]);
		}
	}

	void SetVolumeForAll(float volume) {
		for (int i = 0; i < Size; i++) {
			sounds[i].SetVolume(volume);
		}
	}

	int Play() {
		return sounds[Rand::roll(Size)].Play();
	}

	int Play(vec source, vec listener, float silenceDistance = 300.f) const {
		return sounds[Rand::roll(Size)].Play(source, listener, silenceDistance);
	}

	const Sound& operator[](int i) const {
		return sounds[i];
	}

	constexpr size_t size() const {
		return Size;
	}
private:
	std::array<Sound, Size> sounds;
};
