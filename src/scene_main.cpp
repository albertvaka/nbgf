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

const float kImmunityTime = 2.5f;
const int kInitialLives = 3;
SceneMain::SceneMain()
	: ship()
{
	Particles::Init();
}

void SceneMain::EnterScene() 
{
	lives = kInitialLives;
	immunityTimer = 0.f;
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


    if (immunityTimer <= 0) {
		for (Rock* rock : Rock::GetAll()) {
			if (Collide(ship.Bounds(), rock->Bounds())) {
				lives--;
				immunityTimer = kImmunityTime;
			}
		}
	} else {
		immunityTimer -= dt;
	}


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
	Window::Draw(Assets::rockTexture, Camera::Bounds()); // Texture unused by the shader
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

	ship.Bounds().DebugDraw();

	if (immunityTimer > 0) {
		if (((int)((immunityTimer)*12))%3) {
			Assets::tintShader.Activate();
			Assets::tintShader.SetUniform("flashColor", 1.f, 0.5f, 0.5f, 0.5f);
		}
	}
	ship.Draw();
	Assets::tintShader.Deactivate();

	Camera::InScreenCoords::Begin();
	for (int i = 0; i < lives; i++) {
		Window::Draw(Assets::heartTexture, Window::GAME_WIDTH - 80 - i*70, 20)
		.withScale(0.3f, 0.3f);
	}
	Camera::InScreenCoords::End();


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
