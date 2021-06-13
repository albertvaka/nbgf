#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "Chain.h"
#include "City.h"
#include "ChainUtils.h"
#include "BaseEnemy.h"


struct ChainNode;
struct EnemiesController;


struct SceneMain : Scene {

	ChainUtils::tNodesContainer mUnchainedNodes;
	Chain mChain;
	City mCity;

	Text mRemainingUnchained;
	Text mLevelCounter;

	int mScoreValue = 0;
	Text mScoreText;

	SceneMain();
	~SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

private:
	ChainNode* GenerateNode(vec&& aPosition);
	EnemiesController* mEnemiesController;
};
