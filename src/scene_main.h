#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "Chain.h"
#include "City.h"
#include "ChainUtils.h"
#include "BaseEnemy.h"
#include "text.h"


struct ChainNode;
struct EnemiesController;


struct SceneMain : Scene {

	ChainUtils::tNodesContainer mUnchainedNodes;
	Chain mChain;
	City mCity;

	Text mRemainingUnchained;
	Text mGameWinText;

	int mScoreValue = 0;
	Text mScoreText;
	bool showInstructions = true;

	float arrowBounce;

	SceneMain();
	~SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void StartScene();
	void EndScene();

private:
	ChainNode* GenerateNode(vec&& aPosition);
	EnemiesController* mEnemiesController;

	Text timerText;
	float timer;

	bool gameOver;
	bool gameWin;
	Text gameOverText;
	Text restartText;
};
