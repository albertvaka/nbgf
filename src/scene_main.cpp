#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "camera.h"
#include "assets.h"
#include "collide.h"
#include "debug.h"

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

	AddEnemies(20);
}

SceneMain::~SceneMain()
{
	for (auto it : mUnchainedNodes)
	{
		delete(it.second);
	}
	for (auto enemy : enemies)
	{
		delete(enemy);
	}

	enemies.clear();
	mUnchainedNodes.clear();
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
			it->second->UpdateUnchained(dt);
			++it;
		}
	}	

	mChain.Update(dt);


	UpdateEnemies(dt);
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

	DrawEnemies();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		
		ImGui::End();
	}
#endif

}

void SceneMain::UpdateEnemies(float dt)
{
	if (enemies.empty()) {
		return;
	}

	for (const auto& enemy : enemies) {		
		enemy->Update(dt);
	}

	DestroyEnemies();
}

void SceneMain::DrawEnemies()
{
	if (enemies.empty()) {
		return;
	}

	for (const auto& enemy : enemies) {
		enemy->Draw();
	}
}

void SceneMain::DestroyEnemies()
{
	enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const BaseEnemy* x)
		{
			return !x->alive;
		}), enemies.end());
}

void SceneMain::AddEnemies(int count)
{
	for (int i = 0; i < count; ++i) {
		enemies.push_back(new BaseEnemy(0, 200));
	}
}

ChainNode* SceneMain::GenerateNode(vec&& aPosition)
{
	return new ChainNode(std::move(aPosition));
}
