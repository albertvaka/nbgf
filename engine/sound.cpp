#include "sound.h"

#include <SDL_mixer.h>
#include <SDL_assert.h>
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

void Sound::SetVolume(float v) { // between 0 and 1
	SDL_assert(v >= 0.f && v <= 1.f);
	Mix_VolumeChunk(sound, v * 128);
}

float Sound::Volume() const {
	return Mix_VolumeChunk(sound, -1) / 128.f;
}

int Sound::Play() const {
	if (Mix_VolumeChunk(sound, -1) == 0) return -1;
	return Mix_PlayChannel(-1, sound, 0);
}

int Sound::Play(vec source, vec listener, float silenceDistance) const {
	float distance = listener.Distance(source);
	if (distance >= silenceDistance) {
		return -1;
	}
	Uint8 intDistance = (Uint8)(distance*(255.f/silenceDistance));
	Sint16 angle = (Sint16)(source.AngleDegs(listener) + 90.f + 360.f);
	int channel = Play();
	if (channel >= 0) {
		Mix_SetPosition(channel, angle, intDistance); // Unlike with Mix_Volume(), the next sound that plays on the same channel won't have this effect set
	}
	return channel;
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
