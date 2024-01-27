#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "bullet.h"
#include "doctor.h"
#include "patient.h"
#include "collide.h"
#include "debug.h"
#include "tiled_tilemap.h"
#include "tiled_objects_entities.h"

SceneMain::SceneMain()
	: map(Tiled::TileMap::Size.x, Tiled::TileMap::Size.y, Assets::spritesheetTexture)
	, player()
	//, alienPartSys(Assets::invadersTexture)
	, deadAliensText(Assets::font_30, Assets::font_30_outline)
{
	map.LoadFromTiled<Tiled::TileMap>();


	deadAliensText.SetString("Hola");
	deadAliensText.SetFillColor(0, 0, 0);
	deadAliensText.SetOutlineColor(255, 255, 0);
	/*
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
	alienPartSys.scale_vel = -0.2f;*/
}

void SceneMain::EnterScene() 
{
	Camera::SetZoom(0.5f, false);
	for (const BoxBounds& room : Tiled::Entities::room) {
		new Doctor(Rand::VecInRange(room*0.7f));
	}
	new Doctor(Tiled::Entities::single_waiting.Center());
	new Patient(Tiled::Entities::single_waiting.Center()+vec(200,0));
}

void SceneMain::ExitScene()
{
	Bullet::DeleteAll();
	Doctor::DeleteAll();
}

void SceneMain::Update(float dt)
{
#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F6;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		return;
	}
#endif

	player.Update(dt);

	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
	}

	for (Patient* a : Patient::GetAll()) {
		a->Update(dt);
	}

	for (Doctor* a : Doctor::GetAll()) {
		a->Update(dt);
	}

	Bullet::DeleteNotAlive();
	Doctor::DeleteNotAlive();
}

void SceneMain::Draw()
{
	Window::Clear(120, 120, 120);

	map.Draw();

	std::vector<BoxEntity*> draws;
	draws.reserve(Doctor::GetAll().size() + Patient::GetAll().size() + 1);
	draws.push_back(&player);
	draws.insert(draws.end(), Doctor::GetAll().begin(), Doctor::GetAll().end());
	draws.insert(draws.end(), Patient::GetAll().begin(), Patient::GetAll().end());
	std::sort(draws.begin(), draws.end(), [](const BoxEntity* a, const BoxEntity* b)
	{
		return a->sortY < b->sortY;
	});
	for (const BoxEntity* a : draws) {
		a->Draw();
		a->Bounds().DebugDraw(255, 0, 0);
	}

	for (const Bullet* b : Bullet::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255, 0, 0);
	}

	//Draw GUI
	Camera::InScreenCoords::Begin();
	Window::Draw(deadAliensText, vec(Camera::InScreenCoords::Center().x, 30))
		.withOrigin(deadAliensText.Size()/2)
		.withScale(0.666f);
	Camera::InScreenCoords::End();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		//if (ImGui::SliderInt("level", &currentLevel, 1, 20)) {
		//};
		ImGui::End();
	}

	//alienPartSys.DrawImGUI();
#endif

}
