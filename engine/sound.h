#pragma once

struct Mix_Chunk;

struct Sound
{
	void Load(const char* path);
	~Sound();

	void SetVolume(float v); //from 0 to 100
	int Play(); //returns a channel id

	static void Stop(int channel_id);

private:
	Mix_Chunk* sound = nullptr;
};

