#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"

struct SceneMain : Scene {

	Player mPlayer;
	int mScoreValue = 0;
	Text mScoreText;

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void SpawnAliens();


};
