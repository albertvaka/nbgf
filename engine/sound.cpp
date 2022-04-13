#include "sound.h"

#include <SDL_mixer.h>
#include <cassert>

#include "debug.h"

void Sound::Load(const char* path) {
	if (sound) Mix_FreeChunk(sound);
	sound = Mix_LoadWAV(path); // Can also load OGG despite the name
	if (!sound) {
		Debug::out << "Unable to load sound '" << path << "': " << Mix_GetError();
		assert(false);
	}
}

Sound::~Sound() {
	if (sound) Mix_FreeChunk(sound);
}

void Sound::SetVolume(float v) { //from 0 to 100
	SDL_assert(v >= 0.f && v <= 100.f);
	Mix_VolumeChunk(sound, v * (128 / 100.f));
}

float Sound::Volume() {
	return Mix_VolumeChunk(sound, -1) * (100 / 128.f);
}

int Sound::Play() const {
	if (Mix_VolumeChunk(sound, -1) == 0) return -1;
	return Mix_PlayChannel(-1, sound, 0);
}

int Sound::PlayInLoop() const {
	if (Mix_VolumeChunk(sound, -1) == 0) return -1;
	return Mix_PlayChannel(-1, sound, -1);
}

void Sound::Stop(int channel) {
	if (channel < 0) return;
	Mix_HaltChannel(channel);
}

bool Sound::Playing(int channel) {
	if (channel < 0) return false; // for sdl_mixer, -1 means all channels
	return Mix_Playing(channel);
}
