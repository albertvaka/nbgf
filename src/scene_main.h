#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"

struct SceneMain : Scene {

	Player player;
	PartSys alienPartSys;
	int currentLevel = 1;
	int deadAliens = 0;
	Text deadAliensText;

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void SpawnAliens();


};
