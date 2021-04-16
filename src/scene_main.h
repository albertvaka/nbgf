#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"

struct MainScene : Scene {

	Player player;
	PartSys alienPartSys;
	int currentLevel;
	int deadAliens;
	Text deadAliensText;

	MainScene();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void SpawnAliens();


};
