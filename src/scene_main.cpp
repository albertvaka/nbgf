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
	/*auto* Node = GenerateNode(vec(Window::GAME_WIDTH * 0.1, Window::GAME_HEIGHT * 0.5));
	mChain.AddNode(Node, nullptr, nullptr);

	auto* Node2 = GenerateNode(vec(Window::GAME_WIDTH * 0.2, Window::GAME_HEIGHT * 0.7));
	mUnchainedNodes.emplace(Node2->myId, Node2);*/

	//UNCOMMENT THIS DO HAVE AN INITIAL CHAIN
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
	mChain.myLeftMaster = startingNodes -1U;
	

	mScoreText.SetString("Kill the invaders");
	mScoreText.SetFillColor(0, 0, 0);
	mScoreText.SetOutlineColor(255, 255, 0);
}

SceneMain::~SceneMain()
{
	for (auto it : mUnchainedNodes)
	{
		delete(it.second);
	}
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
