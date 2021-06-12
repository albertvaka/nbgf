#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
//#include "Chain.h"
#include <vector>

struct ChainNode;

struct SceneMain : Scene {

	std::vector<ChainNode*> mChainNodes; //All possible chain nodes available in game

	int mScoreValue = 0;
	Text mScoreText;

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};
