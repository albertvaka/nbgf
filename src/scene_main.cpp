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
#include "ChainUtils.h"

SceneMain::SceneMain()
	: mChainNodes()
	, mScoreText(Assets::font_30, Assets::font_30_outline)
{

	mChainNodes.reserve(50U);
	int startingNodes = 15;
	for (size_t i = 0U; i < startingNodes; ++i) {
		mChainNodes.push_back(new ChainNode(vec(Window::GAME_WIDTH * 0.2, Window::GAME_HEIGHT * 0.5)));
	}

	for (size_t i = 0U; i < startingNodes; ++i) 
	{
		ChainNode* previousNode(nullptr);
		ChainNode* currentNode = mChainNodes[i];
		ChainNode* nextNode(nullptr);
		if (i != 0U)
		{
			previousNode = mChainNodes[i - 1U];
		}
		if (i != startingNodes - 1U)
		{
			nextNode = mChainNodes[i + 1U];
		}
		ChainUtils::AddToChain(currentNode, previousNode, nextNode);
	}

	mScoreText.SetString("Kill the invaders");
	mScoreText.SetFillColor(0, 0, 0);
	mScoreText.SetOutlineColor(255, 255, 0);
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

	std::for_each(mChainNodes.begin(), mChainNodes.end(), [dt](ChainNode* aNode) {
		aNode->Update(dt);
	});

}

void SceneMain::Draw()
{
	Window::Clear(0, 0, 0);

	Window::Draw(Assets::backgroundTexture, Camera::Center())
		.withOrigin(Assets::backgroundTexture->w/2, Assets::backgroundTexture->h/2);

	for (int i = 0; i < mChainNodes.size(); ++i) {
		if (mChainNodes[i] != nullptr) {
			mChainNodes[i]->Draw();
		}
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
