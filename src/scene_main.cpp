#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "camera.h"
#include "assets.h"
#include "collide.h"
#include "debug.h"

float kAlienMinDistance = 300;
float kAlienMaxDistance = 400;

SceneMain::SceneMain()
	: mChain()
	, mScoreText(Assets::font_30, Assets::font_30_outline)
{
	/*
	mScoreText.SetString("Kill the invaders");
	mScoreText.SetFillColor(0, 0, 0);
	mScoreText.SetOutlineColor(255, 255, 0);*/
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

	mChain.Update(dt);

}

void SceneMain::Draw()
{
	Window::Clear(0, 0, 0);

	Window::Draw(Assets::backgroundTexture, Camera::Center())
		.withOrigin(Assets::backgroundTexture->w/2, Assets::backgroundTexture->h/2);

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
