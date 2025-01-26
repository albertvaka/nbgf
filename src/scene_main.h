#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "rand.h"
#include "sound.h"
#include "gaemtilemap.h"


const float MIN_TEMPS_NOTA = 0.15;
const float MAX_TEMPS_NOTA = 0.25;

struct NotePlaying {
	MultiSound<4>* sound;
	int channel = -1;
	float remainingTime = -1;
	void Play() {
		remainingTime = Rand::rollf(MIN_TEMPS_NOTA, MAX_TEMPS_NOTA);
		if (channel == -1) {
			channel = sound->Play();
		}
	}
	void Update(float dt) {
		if (channel != -1) {
			remainingTime -= dt;
			if (remainingTime <= 0.f) {
				Sound::Stop(channel);
				channel = -1;
			} else if (!Sound::Playing(channel)) {
				channel = sound->Play();
			}
		}
	}
};

struct SceneMain : Scene {

	Text scoreText[2];
	Text comboText[2];
	PartSys alienPartSys;

	NotePlaying notePlaying[2][4];

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
	void Note(int player, int num);
	void updateScore(int player);
	std::vector<int> notes;
	void LoadSong();
	void playerFloatingText(int player, std::string text, bool bad = false);

	int loop = 0;
	float currentTime = 0;
	int currentNote = 0;
	const float SECONDS_PER_BEAT = 0.5f; // song is 120 bpm
	const float MARGIN = 0.1f;
	int combo[2];
	int score[2];
	vec playerTextPos = { 550, 150 };
	float rotoArea = 120;
	BoxBounds collider = BoxBounds(400, 270, 800, 30);
	float rotoScale = 0.85f;
	float sceneClock = 0;
	float timeToFinish = 0;

};
