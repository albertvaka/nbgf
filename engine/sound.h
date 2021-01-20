#pragma once

struct Mix_Chunk;

struct Sound
{
	void Load(const char* path);
	~Sound();

	int Play(); //returns a channel id
	static void Stop(int channel_id);

	void SetVolume(float v); //from 0 to 100
	float Volume(); //from 0 to 100

private:
	Mix_Chunk* sound = nullptr;
};

