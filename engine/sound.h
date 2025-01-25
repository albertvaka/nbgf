#pragma once

#include <array>
#include "rand.h"
#include "vec.h"
#include "debug.h"

#include <SDL_mixer.h>

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
	
	void LoadAsParts(const char* path) {
		sounds[0].Load(path);
		Mix_Chunk* chunk = sounds[0].sound;
		int totalLen = chunk->alen;

		for (int i = 1; i < Size; i++) {
			int position = i * ((totalLen / Size) / 16)*16;
			Debug::out << "position: " << position << " totalLen: " << totalLen;
			Mix_Chunk* subChunk = new Mix_Chunk;
			subChunk->allocated = 0;
			subChunk->abuf = chunk->abuf + position;
			subChunk->alen = chunk->alen - position;
			subChunk->volume = chunk->volume;
			
			sounds[i].sound = subChunk;
		}
	}

	int Play() {
		int a = Rand::roll(Size);
		Debug::out << a;
		return sounds[a].Play();
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
