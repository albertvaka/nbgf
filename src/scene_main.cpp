#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "collide.h"
#include "window.h"
#include "camera.h"
#include "particles.h"
#include "scene_manager.h"
#include "musicplayer.h"
#include "fx.h"
#include "debug.h"

#include "goals.h"
#include "fish.h"
#include "rock.h"
#include "chunks.h"
#include "bullet.h"

#include <cmath>

const int kInitialLives = 3;

extern float mainClock;

SceneMain::SceneMain()
	: ship()
{
	Particles::Init();
}

void SceneMain::EnterScene() 
{
	Fx::BeforeEnterScene();

	lives = kInitialLives;

	Debug::DebugDrawScale = 3.f;
	ship.Reset();

	Chunks::SpawnInitial(veci(0, 0));

	goals.Reset();

	Fx::ScreenTransition::Start(Assets::fadeInDiamondsShader);
	Fx::FreezeImage::SetAlternativeUpdateFnWhileFrozen([](float dt) {

	});
}

void SceneMain::ExitScene()
{
	Particles::ClearAll();
	Rock::DeleteAll();
	Fish::DeleteAll();
}

void SceneMain::Update(float dt)
{
	MusicPlayer::ToggleMusicWithM();

	Fx::Update(dt);
	if (Fx::FreezeImage::IsFrozen()) {
		// Fx::FreezeImage's alternate update function has already run at this point
		return;
	}
	if (Fx::ScreenTransition::IsActive()) {
		return;
	}

	if (Fx::ScreenTransition::IsJustFinished()) {
		if (Fx::ScreenTransition::Current() != &Assets::fadeInDiamondsShader) {
			SceneManager::RestartScene();
		}
		return;
	}

	veci lastChunk = Chunks::GetChunk(ship.pos);
	
	bool hitRock = ship.Update(dt);

	if (hitRock) {
		lives--;
		if (lives == 0) {
			goals.GameOver();
		}
	}

	Fish::UpdateAll(dt);

	if (goals.Update(dt)) {
		// Will restart the scene
		vec normalizedPlayerPos = Camera::WorldToScreen(ship.pos) / Camera::InScreenCoords::Size();
		Assets::fadeOutCircleShader.Activate(); // Must be active to set uniforms
		Assets::fadeOutCircleShader.SetUniform("normalizedTarget", normalizedPlayerPos);
		Assets::fadeOutDiamondsShader.Deactivate();
		Fx::ScreenTransition::Start(Assets::fadeOutCircleShader);
	}

	Particles::UpdateAll(dt);

	veci currentChunk = Chunks::GetChunk(ship.pos);
	Chunks::Update(lastChunk, currentChunk);
}

void SceneMain::Draw()
{
	Window::Clear(0.1 * 255, 0.45 * 255, 0.73 * 255);

	for (Fish* e : Fish::GetAll()) {
		e->Draw();
	}

	Assets::seaShader.Activate();
	Assets::seaShader.SetUniform("offset", Camera::Center());
	Assets::seaShader.SetUniform("iResolution", Window::GetViewportScaledResolution());
	Assets::seaShader.SetUniform("windowScale", Window::GetViewportScale());
	Assets::seaShader.SetUniform("zoom", Camera::Zoom());
	Assets::seaShader.SetUniform("iTime", mainClock);
	Window::Draw(Assets::rockTexture, Camera::Bounds()); // Texture unused by the shader
	Assets::seaShader.Deactivate();

	ship.DrawStroke();

	for (Rock* e : Rock::GetAll()) {
		e->Draw();
	}

	Assets::outlineShader.Activate();
	Assets::outlineShader.SetUniform("iTime", mainClock);
	for (Rock* e : Rock::GetAll()) {
		e->DrawFoam();
	}
	Assets::outlineShader.Deactivate();

	Particles::waterTrail.Draw();

	goals.Draw();

	ship.Draw();

	Camera::InScreenCoords::Begin();
	for (int i = 0; i < lives; i++) {
		Window::Draw(Assets::heartTexture, Window::GAME_WIDTH - 80 - i*70, 20)
		.withScale(0.3f, 0.3f);
	}
	Camera::InScreenCoords::End();

	goals.DrawGui();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		if (ImGui::Button("Goal")) {
			goals.GotGoal();
		}
		ImGui::Text("Input device: %s", magic_enum::enum_name(Input::PreferredUserInputDevice).data());
		if (ImGui::Button("Fullscreen")) {
			Debug::out << "Fullscreen" << Window::IsFullScreen();
			Window::SetFullScreen(!Window::IsFullScreen());
		}
		ImGui::End();
	}

	//Particles::waterTrail.DrawImGUI("water trail");
#endif

	Fx::AfterDraw();
}
