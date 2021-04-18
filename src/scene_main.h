#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"

struct MainScene : Scene {

	Player player;
	int currentLevel;
	float timer;
	Text timerText;

	MainScene() : MainScene(1) { };

	MainScene(int level);

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};