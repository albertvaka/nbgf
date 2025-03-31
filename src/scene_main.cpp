#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "bullet.h"
#include "collide.h"
#include "rock.h"
#include "particles.h"
#include "debug.h"
#include <cmath>

SceneMain::SceneMain()
	: ship()
{
	Particles::Init();
	outlinedSprites = Window::CreateTexture(Window::GetGameResolution());

}

void SceneMain::EnterScene() 
{
	Debug::DebugDrawScale = 3.f;
	ship.Reset();
	Camera::SetZoom(0.7);
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			Rock::SpawnInChunk(i, j);
		}
	}
}

void SceneMain::ExitScene()
{
	Particles::ClearAll();
	Rock::DeleteAll();
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

	veci lastChunk = Rock::GetChunk(ship.pos);
	ship.Update(dt);
	veci currentChunk = Rock::GetChunk(ship.pos);
	Rock::DebugDrawChunks(currentChunk);
	Rock::Spawn(currentChunk, lastChunk);

	Particles::UpdateAll(dt);
}

void SceneMain::Draw()
{
	Window::Clear(0, 0, 0);

	Assets::seaShader.Activate();
	Assets::seaShader.SetUniform("offset", Camera::Center());
	Assets::seaShader.SetUniform("iResolution", Window::GetViewportScaledResolution());
	Assets::seaShader.SetUniform("windowScale", Window::GetViewportScale());
	Assets::seaShader.SetUniform("zoom", Camera::Zoom());
	Assets::seaShader.SetUniform("iTime", mainClock);
	Window::Draw(outlinedSprites, Camera::Bounds());
	Assets::seaShader.Deactivate();

	ship.DrawStroke();

	for (Rock* rock : Rock::GetAll()) {
		rock->Draw();
	}

	Assets::outlineShader.Activate();
	Assets::outlineShader.SetUniform("offset", Camera::Center());
	Assets::outlineShader.SetUniform("iResolution", Window::GetViewportScaledResolution());
	Assets::outlineShader.SetUniform("windowScale", Window::GetViewportScale());
	Assets::outlineShader.SetUniform("zoom", Camera::Zoom());
	Assets::outlineShader.SetUniform("iTime", mainClock);
	for (Rock* rock : Rock::GetAll()) {
		rock->DrawFoam();
	}
	Assets::outlineShader.Deactivate();

	Particles::waterTrail.Draw();

	ship.Draw();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::End();
	}

	Particles::waterTrail.DrawImGUI();
#endif
}
