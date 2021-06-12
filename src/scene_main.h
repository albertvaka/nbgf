#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "Chain.h"
#include "ChainUtils.h"


struct ChainNode;

struct SceneMain : Scene {

	ChainUtils::tNodesContainer mUnchainedNodes;
	Chain mChain;

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
};
