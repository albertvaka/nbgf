#include "scene_main.h"
#include "raw_input.h"
#include <iostream>
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "camera.h"
#include "assets.h"
#include "collide.h"
#include "debug.h"
#include "EnemiesController.h"

#include "ChainNode.h"


SceneMain::SceneMain()
	: mUnchainedNodes()
	, mChain()
	, mScoreText(Assets::font_30, Assets::font_30_outline)
{
	//COMMENT THIS DO HAVE AN INITIAL CHAIN
	//TODO Would be cool to have this in a factory/chainNodesSpawner class and set from there the ids as well
	auto* Node = GenerateNode(vec(Window::GAME_WIDTH * 0.1, Window::GAME_HEIGHT * 0.5));
	mChain.AddNode(Node, nullptr, nullptr);

	int startingUnchainedNodes = 15;

	for (int i = 0; i < startingUnchainedNodes; ++i) {
		int randomX = rand() % Window::GAME_WIDTH;
		int randomY = rand() % Window::GAME_HEIGHT;
		auto* newNode = GenerateNode(vec(randomX,  randomY));
		mUnchainedNodes.emplace(newNode->myId, newNode);
	}


	//UNCOMMENT THIS DO HAVE AN INITIAL CHAIN
	/*
	size_t startingNodes = 15U;
	std::vector<ChainNode*> chainNodes;
	for (size_t i = 0U; i < startingNodes; ++i) {
		chainNodes.push_back(GenerateNode(vec(Window::GAME_WIDTH * 0.2, Window::GAME_HEIGHT * 0.5)));
	}
	for (size_t i = 0U; i < startingNodes; ++i)
	{
		ChainNode* previousNode(nullptr);
		ChainNode* currentNode = chainNodes[i];
		ChainNode* nextNode(nullptr);
		if (i != 0U)
		{
			previousNode = chainNodes[i - 1U];
		}
		if (i != startingNodes - 1U)
		{
			nextNode = chainNodes[i + 1U];
		}
		mChain.AddNode(currentNode, previousNode, nextNode);
	}
	*/

	//Enemies----
	mEnemiesController = new EnemiesController();
	mEnemiesController->Awake();
}

SceneMain::~SceneMain()
{
	for (auto it : mUnchainedNodes)
	{
		delete(it.second);
	}

	mUnchainedNodes.clear();

	mEnemiesController->CleanUp();

	delete(mEnemiesController);
}

void SceneMain::EnterScene() 
{
	
}

void SceneMain::ExitScene()
{
	
}

void SceneMain::Update(float dt)
{
#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		EnterScene();
		return;
	}
#endif

	for (auto it = mUnchainedNodes.begin(); it != mUnchainedNodes.end();) 
	{
		if (mChain.TryToJoin(it->second))
		{
			it = mUnchainedNodes.erase(it);
		}
		else		
		{
			it->second->UpdateUnchained(dt, mChain.GetNodes());
			++it;
		}
	}	

	mChain.Update(dt);	

	mEnemiesController->Update(dt);

	//Retrieve nodes to unchain from chain
	const auto& nodesToUnchain = mChain.GetNodesToUnchain();
	for (auto& nodeToUnchain : nodesToUnchain)
	{
		mUnchainedNodes.emplace(nodeToUnchain->myId, nodeToUnchain);
		nodeToUnchain->ActivateChainCooldown();
	}
	mChain.ResetNodesToUnchain();
}

void SceneMain::Draw()
{
	Window::Clear(0, 0, 0);

	Window::Draw(Assets::backgroundTexture, Camera::Center())
		.withOrigin(Assets::backgroundTexture->w/2, Assets::backgroundTexture->h/2)
		.withScale(Window::MAP_SCALE);

	for (auto& unchainedIt : mUnchainedNodes)
	{
		unchainedIt.second->Draw();
	}
		
	mChain.Draw();

	mEnemiesController->DrawEnemies();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		
		ImGui::End();
	}
#endif

}

ChainNode* SceneMain::GenerateNode(vec&& aPosition)
{
	return new ChainNode(std::move(aPosition));
}
