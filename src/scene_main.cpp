#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "bullet.h"
#include "alien.h"
#include "collide.h"
#include "debug.h"

float kAlienMinDistance = 300;
float kAlienMaxDistance = 400;

SceneMain::SceneMain()
	: ship()
{
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

	Camera::SetCenter(ship.pos);

	ship.Update(dt);
}

void SceneMain::Draw()
{
	Window::Clear(0, 162, 232);

	Window::Draw(Assets::backgroundTexture, vec(0,0))
		.withOrigin(Assets::backgroundTexture->w / 2, Assets::backgroundTexture->h / 2);

	ship.DrawStroke();
	ship.Draw();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::End();
	}
#endif

}
