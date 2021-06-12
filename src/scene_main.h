#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "Chain.h"

struct SceneMain : Scene {

	Chain mChain;
	int mScoreValue = 0;
	Text mScoreText;

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};
