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

};
