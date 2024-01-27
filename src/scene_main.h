#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"
#include "gaemtilemap.h"

struct SceneMain : Scene {

	static int maxPatients;

	Player player;
	GaemTileMap map;
	Text deadAliensText;
	float patientIncreaseTimer;
	float timerSpawnPatients;

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};
