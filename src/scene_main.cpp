#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "fx.h"
#include "bullet.h"
#include "collide.h"
#include "debug.h"
#include "tiled_tilemap.h"
#include "tiled_objects_entities.h"

SceneMain::SceneMain()
	: map(Tiled::TileMap::Size.x, Tiled::TileMap::Size.y, Assets::spritesheetTexture)
	//, alienPartSys(Assets::invadersTexture)
	, scoreText(Assets::font_30, Assets::font_30_outline)
{
	//map.LoadFromTiled<Tiled::TileMap>();

	notePlaying[0][0].sound = &Assets::note1p1;
	notePlaying[0][1].sound = &Assets::note2p1;
	notePlaying[0][2].sound = &Assets::note3p1;
	notePlaying[0][3].sound = &Assets::note4p1;
	notePlaying[1][0].sound = &Assets::note1p2;
	notePlaying[1][1].sound = &Assets::note2p2;
	notePlaying[1][2].sound = &Assets::note3p2;
	notePlaying[1][3].sound = &Assets::note4p2;

	scoreText.SetFillColor(0, 0, 0);
	scoreText.SetOutlineColor(255, 255, 0);
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
	Fx::BeforeEnterScene();
	Fx::FullscreenShader::SetShader([]() {
		Assets::underwaterShader.Activate();
		Assets::underwaterShader.SetUniform("time", mainClock);
	});
}


void SceneMain::ExitScene()
{
	Bullet::DeleteAll();
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

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			notePlaying[i][j].Update(dt);
		}
	}

	if (Input::IsJustPressed(0, GameKeys::NOTE_1)) { Note(0, 0); }
	if (Input::IsJustPressed(1, GameKeys::NOTE_1)) { Note(1, 0); }
	if (Input::IsJustPressed(0, GameKeys::NOTE_2)) { Note(0, 1); }
	if (Input::IsJustPressed(1, GameKeys::NOTE_2)) { Note(1, 1); }
	if (Input::IsJustPressed(0, GameKeys::NOTE_3)) { Note(0, 2); }
	if (Input::IsJustPressed(1, GameKeys::NOTE_3)) { Note(1, 2); }
	if (Input::IsJustPressed(0, GameKeys::NOTE_4)) { Note(0, 3); }
	if (Input::IsJustPressed(1, GameKeys::NOTE_4)) { Note(1, 3); }

	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
	}
	Bullet::DeleteNotAlive();
}

void SceneMain::Note(int player, int num) {
	notePlaying[player][num].Play();
}

void SceneMain::Draw()
{
	Fx::FullscreenShader::Begin();
	Window::Clear(0, 0, 0, 255);
	for (const Bullet* b : Bullet::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255, 0, 0);
	}
	Fx::FullscreenShader::End();

	//Draw GUI
	Camera::InScreenCoords::Begin();
	Window::Draw(scoreText, vec(Camera::InScreenCoords::Center().x, 20))
		.withOrigin(scoreText.Size()/2)
		.withScale(0.666f);
	Camera::InScreenCoords::End();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		ImGui::End();
	}

	//alienPartSys.DrawImGUI();
#endif

	Fx::AfterDraw();
}
