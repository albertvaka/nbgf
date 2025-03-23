#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "bullet.h"
#include "alien.h"
#include "collide.h"
#include "particles.h"
#include "debug.h"

float kAlienMinDistance = 300;
float kAlienMaxDistance = 400;

SceneMain::SceneMain()
	: ship()
{
	Particles::Init();
}

void SceneMain::EnterScene() 
{
	Debug::DebugDrawScale = 3.f;
	ship.Reset();
	Camera::SetZoom(0.7);
}

void SceneMain::ExitScene()
{
	Particles::ClearAll();
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

	ship.Update(dt);
	Particles::UpdateAll(dt);
}

void SceneMain::Draw()
{
	Window::Clear(0, 162, 232);

	Assets::seaShader.Activate();
	Assets::seaShader.SetUniform("offset", Camera::Center());
	Assets::seaShader.SetUniform("iResolution", Window::GetViewportScaledResolution());
	Assets::seaShader.SetUniform("windowScale", Window::GetViewportScale());
	Assets::seaShader.SetUniform("zoom", Camera::Zoom());
	Assets::seaShader.SetUniform("iTime", mainClock);
	Window::Draw(Assets::backgroundTexture, Camera::Bounds());
	Assets::seaShader.Deactivate();

	ship.DrawStroke();
	Particles::waterTrail.Draw();
	ship.Draw();

	Particles::waterTrail.DrawImGUI();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::End();
	}
#endif

}
