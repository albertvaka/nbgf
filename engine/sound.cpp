#include "sound.h"

#include <SDL_mixer.h>

#include "debug.h"

void Sound::Load(const char* path) {
	if (sound) Mix_FreeChunk(sound);
	sound = Mix_LoadWAV(path);
	if (!sound) {
		Debug::out << "Unable to load sound '" << path << "': " << Mix_GetError();
	}
}

Sound::~Sound() {
	if (sound) Mix_FreeChunk(sound);
}

void Sound::SetVolume(float v) { //from 0 to 100
	Mix_VolumeChunk(sound, v * (128 / 100.f));
}

float Sound::Volume() {
	return Mix_VolumeChunk(sound, -1) * (100 / 128.f);
}

int Sound::Play() {
	if (Mix_VolumeChunk(sound, -1) == 0) return -1;
	return Mix_PlayChannel(-1, sound, 0);
}

void Sound::Stop(int channel) {
	Mix_HaltChannel(channel);
}
