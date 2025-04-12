#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "bullet.h"
#include "collide.h"
#include "window.h"
#include "camera.h"
#include "rock.h"
#include "particles.h"
#include "debug.h"
#include "chunks.h"
#include <cmath>

extern float mainClock;

SceneMain::SceneMain()
	: ship()
{
	Particles::Init();
}

void SceneMain::EnterScene() 
{
	Debug::DebugDrawScale = 3.f;
	ship.Reset();

	Chunks::SpawnInitial(veci(0, 0));
}

void SceneMain::ExitScene()
{
	Particles::ClearAll();
	Rock::DeleteAll();
}

void SceneMain::Update(float dt)
{
	veci lastChunk = Chunks::GetChunk(ship.pos);
	ship.Update(dt);

	Particles::UpdateAll(dt);

	veci currentChunk = Chunks::GetChunk(ship.pos);
	Chunks::Update(lastChunk, currentChunk);
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
	Window::Draw(Assets::rockTexture, Camera::Bounds()); // Texture unused by the shader
	Assets::seaShader.Deactivate();

	ship.DrawStroke();

	for (Rock* rock : Rock::GetAll()) {
		rock->Draw();
	}

	Assets::outlineShader.Activate();
	Assets::outlineShader.SetUniform("iTime", mainClock);
	for (Rock* rock : Rock::GetAll()) {
		rock->DrawFoam();
	}
	Assets::outlineShader.Deactivate();

	Particles::waterTrail.Draw();

	auto [frontBounds, middleBounds, backBounds] = ship.Bounds();
	frontBounds.DebugDraw();
	middleBounds.DebugDraw();
	backBounds.DebugDraw();

	ship.Draw();

	Camera::InScreenCoords::Begin();
	for (int i = 0; i < ship.lives; i++) {
		Window::Draw(Assets::heartTexture, Window::GAME_WIDTH - 80 - i*70, 20)
		.withScale(0.3f, 0.3f);
	}
	Camera::InScreenCoords::End();


#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::Text("Input device: %s", magic_enum::enum_name(Input::PreferredUserInputDevice).data());
		if (ImGui::Button("Fullscreen")) {
			Debug::out << "Fullscreen" << Window::IsFullScreen();
			Window::SetFullScreen(!Window::IsFullScreen());
		}
		ImGui::End();
	}

	//Particles::waterTrail.DrawImGUI();
#endif
}
