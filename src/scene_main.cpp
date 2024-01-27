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
#include "tiled_tilemap.h"

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
}

void SceneMain::ExitScene()
{
	Bullet::DeleteAll();
	Alien::DeleteAll();
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

	for (Alien* a : Alien::GetAll()) {
		a->Update(dt);
	}

	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
		for (Alien* a  : Alien::GetAll()) {
			if (Collide(a,b)) {

				//deadAliensText.SetString("Kills: " + std::to_string(deadAliens));

				a->alive = false;
				b->alive = false;
			}
		}
	}

	Bullet::DeleteNotAlive();
	Alien::DeleteNotAlive();
}

void SceneMain::Draw()
{
	Window::Clear(120, 120, 120);

	map.Draw();
	player.Draw();

	for (const Alien* a : Alien::GetAll()) {
		a->Draw();
		a->Bounds().DebugDraw(255,0,0);
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
