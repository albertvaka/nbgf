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
	: player(200)
	, alienPartSys(Assets::invadersTexture)
	, deadAliensText(Assets::font_30, Assets::font_30_outline)
{
	deadAliensText.SetString("Kill the invaders");
	deadAliensText.SetFillColor(0, 0, 0);
	deadAliensText.SetOutlineColor(255, 255, 0);

	alienPartSys.AddSprite(AnimLib::ALIEN_1[0].rect);
	alienPartSys.AddSprite(AnimLib::ALIEN_2[0].rect);
	alienPartSys.min_scale = 0.15f;
	alienPartSys.max_scale = 0.4f;
	alienPartSys.max_vel = vec(50,50);
	alienPartSys.min_vel = vec(-50,-50);
	alienPartSys.min_rotation = 0;
	alienPartSys.max_rotation = 360;
	alienPartSys.max_rotation_vel = 100.f;
	alienPartSys.min_rotation_vel = 100.f;
	alienPartSys.scale_vel = -0.2f;
}

void SceneMain::EnterScene() 
{
	SpawnAliens();
}

void SceneMain::SpawnAliens() {
	for (int angle = 0; angle < 360; angle += 360/currentLevel) {
		new Alien(angle, Rand::rollf(kAlienMinDistance, kAlienMaxDistance));
	}
}

void SceneMain::ExitScene()
{
	alienPartSys.Clear();
	Bullet::DeleteAll();
	Alien::DeleteAll();
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

	if (Alien::GetAll().empty()) {
		currentLevel++;
		SpawnAliens();
	}

	player.Update(dt);

	for (Alien* a : Alien::GetAll()) {
		a->Update(dt);
	}

	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
		for (Alien* a  : Alien::GetAll()) {
			if (Collide(a,b)) {
				Debug::out << "KABOOM " << a->pos;

				deadAliens++;
				deadAliensText.SetString("Kills: " + std::to_string(deadAliens));

				alienPartSys.pos = a->pos;
				alienPartSys.AddParticles(10);
				
				a->alive = false;
				b->alive = false;
			}
		}
	}

	Bullet::DeleteNotAlive();
	Alien::DeleteNotAlive();

	alienPartSys.UpdateParticles(dt);

}

void SceneMain::Draw()
{
	Window::Clear(0, 0, 0);

	Window::Draw(Assets::backgroundTexture, Camera::Center())
		.withOrigin(Assets::backgroundTexture->w/2, Assets::backgroundTexture->h/2);

	player.Draw();

	for (const Alien* a : Alien::GetAll()) {
		a->Draw();
		a->Bounds().DebugDraw(255,0,0);
	}

	for (const Bullet* b : Bullet::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255, 0, 0);
	}

	alienPartSys.Draw();

	Window::Draw(deadAliensText, vec(Camera::Center().x, Camera::Top() - 30))
		.withOrigin(deadAliensText.Size()/2)
		.withScale(0.666f);

	if (Debug::Draw) {
		Window::DrawPrimitive::Circle(Camera::Center(), kAlienMinDistance, 1, 255, 255, 255);
		Window::DrawPrimitive::Circle(Camera::Center(), kAlienMaxDistance, 1, 255, 255, 255);
	}

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::Text("Angle: %s", Angles::RadsToStr(Camera::Center().AngleRads(player.pos), true).c_str());
		if (ImGui::SliderInt("level", &currentLevel, 1, 20)) {
			Alien::DeleteAll();
			SpawnAliens();
		};
		ImGui::End();
	}

	//alienPartSys.DrawImGUI();
#endif

}
