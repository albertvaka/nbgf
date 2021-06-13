#include <vector>
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
#include "window_draw.h"
#include "musicplayer.h"
#include "input.h"

#include "ChainNode.h"


SceneMain::SceneMain()
	: mUnchainedNodes()
	, mChain()
	, mScoreText(Assets::font_30, Assets::font_30_outline)
	, mCity()
	, timerText(Assets::font_30, Assets::font_30_outline)
	, gameOverText(Assets::font_30, Assets::font_30_outline)
	, restartText(Assets::font_30, Assets::font_30_outline)
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
	mEnemiesController = new EnemiesController(&mChain);
	mEnemiesController->Awake();

	MusicPlayer::SetVolume(50.f);
	MusicPlayer::Play(Assets::gameMusic);

	timerText.SetFillColor(255, 255, 255);
	timerText.SetOutlineColor(0, 0, 0);

	gameOverText.SetString("Game Over");

	restartText.SetString("Press R to restart");

	timer = 30.f;

	gameOver = false;
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

void SceneMain::StartScene()
{
	//COMMENT THIS DO HAVE AN INITIAL CHAIN
	//TODO Would be cool to have this in a factory/chainNodesSpawner class and set from there the ids as well
	mChain.ResetChain(vec(Window::GAME_WIDTH * 0.1, Window::GAME_HEIGHT * 0.5));

	int startingUnchainedNodes = 15;

	for (int i = 0; i < startingUnchainedNodes; ++i) {
		int randomX = rand() % Window::GAME_WIDTH;
		int randomY = rand() % Window::GAME_HEIGHT;
		auto* newNode = GenerateNode(vec(randomX, randomY));
		mUnchainedNodes.emplace(newNode->myId, newNode);
	}


	//Enemies----
	mEnemiesController = new EnemiesController(&mChain);
	mEnemiesController->Awake();

	timer = 5.f;

	gameOver = false;
}

void SceneMain::EndScene()
{
	for (auto it : mUnchainedNodes)
	{
		delete(it.second);
	}

	mUnchainedNodes.clear();

	mEnemiesController->CleanUp();

	delete(mEnemiesController);
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

	if (gameOver)
	{
		if (Input::IsPressed(0, GameKeys::RESTART))
		{
			EndScene();
			StartScene();
		}
		return;
	}

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

	mCity.Update(dt, &mChain);

	//Retrieve nodes to unchain from chain
	const auto& nodesToUnchain = mChain.GetNodesToUnchain();
	for (auto& nodeToUnchain : nodesToUnchain)
	{
		mUnchainedNodes.emplace(nodeToUnchain->myId, nodeToUnchain);
		nodeToUnchain->ActivateChainCooldown();
	}
	mChain.ResetNodesToUnchain();

	timer -= dt;
	timerText.SetString("Time left: " + Mates::to_string_with_precision(timer, 2));

	if (timer <= 0)
	{
		gameOver = true;
	}
}

std::vector<Window::PartialDraw> shadows;
std::vector<Window::PartialDraw> draws;
std::vector<Window::PartialDraw*> drawps;
void SceneMain::Draw()
{
	draws.clear();
	drawps.clear();
	shadows.clear();
	Window::Clear(154, 196, 98);
	// Get PartialDraws to be able to sort
	auto aPds = mCity.PartialDraws();
	shadows.insert(std::end(shadows), std::begin(aPds.first), std::end(aPds.first));
	draws.insert(std::end(draws), std::begin(aPds.second), std::end(aPds.second));

	SDL_Color lDefaultNodeColor; lDefaultNodeColor.r = 255; lDefaultNodeColor.g = 255; lDefaultNodeColor.b = 255; lDefaultNodeColor.a = 255;
	/*
	for (auto& unchainedIt : mUnchainedNodes)
	{
		unchainedIt.second->Draw(lDefaultNodeColor);
	}
	*/
	for (auto& unchainedIt : mUnchainedNodes)
	{
		auto p = unchainedIt.second->PartialDraws(lDefaultNodeColor);
		shadows.push_back(p.first);
		draws.push_back(p.second);
	}

	auto aChainPds = mChain.PartialDraws();
	shadows.insert(std::end(shadows), std::begin(aChainPds.first), std::end(aChainPds.first));
	draws.insert(std::end(draws), std::begin(aChainPds.second), std::end(aChainPds.second));

	for(auto& pd : shadows) {
		pd.DoDraw();
	}
	// Get pointers, sort and draw
	for (Window::PartialDraw& pd : draws) {
		drawps.push_back(&pd);
	}
	std::sort(drawps.begin(), drawps.end(), [](Window::PartialDraw* a, Window::PartialDraw* b) {
		return (a->dest.y) < (b->dest.y);
	});
	for(auto& pd : drawps) {
		pd->DoDraw();
	}
		
	mEnemiesController->DrawEnemies();

	Window::Draw(timerText, vec(Camera::Center().x, 30))
		.withOrigin(timerText.Size() / 2)
		.withScale(1.0f);

	if (gameOver)
	{
		Window::Draw(gameOverText, vec(Camera::Center().x, Camera::Center().y))
			.withOrigin(gameOverText.Size() / 2)
			.withScale(5.0f);

		Window::Draw(restartText, vec(Camera::Center().x, Camera::Center().y+120))
			.withOrigin(restartText.Size() / 2)
			.withScale(1.0f);
	}

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
