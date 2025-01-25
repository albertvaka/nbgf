#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "fx.h"
#include "bullet.h"
#include "musicplayer.h"
#include "collide.h"
#include "debug.h"
#include "rototext.h"
#include "tiled_tilemap.h"
#include "tiled_objects_entities.h"

std::vector<int> notes;
float currentTime = 0;
int currentNote = 0;
const float SECONDS_PER_BEAT = 0.5f; // song is 120 bpm
const float MARGIN = 0.1f;
int combo[2];
int score[2];
BoxBounds collider = BoxBounds(400, 287, 800, 20);

void LoadSong() {
	std::ifstream file("data/score.txt", std::ifstream::in);
	if (file.fail()) {
		Debug::out << "Could not open score.txt for reading: " << strerror(errno);
		return;
	}
	std::string data;
	while (file >> data) {
		data.erase(std::remove_if(data.begin(), data.end(), [](unsigned char c) {
			return std::isspace(c);
		}), data.end());
		if (data.empty()) continue;
		int note = 0;
		if (data[0] != '-') {
			note += 1;
		}
		if (data[1] != '-') {
			note += 2;
		}
		if (data[2] != '-') {
			note += 4;
		}
		if (data[3] != '-') {
			note += 8;
		}
		notes.push_back(note);
	}
	file.close();
}

SceneMain::SceneMain()
	: map(Tiled::TileMap::Size.x, Tiled::TileMap::Size.y, Assets::spritesheetTexture)
	//, alienPartSys(Assets::invadersTexture)
	, scoreText(Assets::font_30, Assets::font_30_outline)
{

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
	LoadSong();

}

void SceneMain::EnterScene() 
{
	Fx::BeforeEnterScene();
	Fx::FullscreenShader::SetShader([]() {
		//Assets::underwaterShader.Activate();
		//Assets::underwaterShader.SetUniform("time", mainClock);
	});

	MusicPlayer::SetVolume(20);
	MusicPlayer::Play(Assets::menuMusic);
	currentTime = 0;
	currentNote = 0;
	score[0] = 0;
	score[1] = 0;
	combo[0] = 0;
	combo[1] = 0;
}

void SceneMain::ExitScene()
{
	Bullet::DeleteAll();
	RotoText::DeleteAll();
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

	currentTime += dt;
	while (currentTime > SECONDS_PER_BEAT) {
		currentTime -= SECONDS_PER_BEAT;
		currentNote++;
		if (currentNote >= notes.size()) {
			currentNote = 0;
		}
		if (notes[currentNote] & 1) {
			new Bullet(0, 0);
		}

		if (notes[currentNote] & 2) {
			new Bullet(0, 1);
		}

		if (notes[currentNote] & 4) {
			new Bullet(0, 2);
		}

		if (notes[currentNote] & 8) {
			new Bullet(0, 3);
		}
	}


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

	for (RotoText* b : RotoText::GetAll()) {
		b->Update(dt);
	}
	RotoText::DeleteNotAlive();
	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
		b->active = Collide(collider, b->Bounds());
		if (b->pos.y < collider.Top()) {
			b->alive = false;
			if (!b->hit[0]) {
				combo[0] = 0;
				auto text = new RotoText(vec(400, 300) + Rand::PosInsideCircle(200), Assets::funk_30, Assets::funk_30_outline);
				text->ShowMessage("Miss");
			}
			if (!b->hit[1]) {
				combo[1] = 0;
				auto text = new RotoText(vec(400, 300) + Rand::PosInsideCircle(200), Assets::funk_30, Assets::funk_30_outline);
				text->ShowMessage("Miss");
			}
		}
	}
	Bullet::DeleteNotAlive();
}


void SceneMain::Note(int player, int note) {
	notePlaying[player][note].Play();
	bool collide = false;
	for (Bullet* b : Bullet::GetAll()) {
		if (!b->hit[player] && b->note == note && Collide(collider, b->Bounds())) {
			collide = true;
			b->hit[player] = true;
		}
		collide = true;
		b->hit[player] = true;
	}
	if (collide) {
		combo[player]++;
		auto text = new RotoText(vec(400, 300) + Rand::PosInsideCircle(200), Assets::funk_30, Assets::funk_30_outline);
		switch (Rand::roll(3)) {
			case 0: text->ShowMessage("yo!"); break;
			case 1: text->ShowMessage("c'mon"); break;
			case 2: text->ShowMessage("yeah"); break;
			default: text->ShowMessage("Yo!!!!!!!"); break;
		}
	}
	else {
		combo[player] = 0;
		auto text = new RotoText(vec(400, 300) + Rand::PosInsideCircle(200), Assets::funk_30, Assets::funk_30_outline);
		text->ShowMessage("Oops!");
	}

}

void SceneMain::Draw()
{
	Fx::FullscreenShader::Begin();
	Window::Clear(100, 100, 230, 255);
	Window::Draw(Assets::seaBgTexture, vec::Zero);
	Window::Draw(Assets::air, vec(715, 0)).withRect(0, mainClock*500.f, Assets::air->w, Window::GAME_HEIGHT);
	Window::Draw(Assets::window, vec(656, 250)).withScale(0.9, 0.9);
	for (const Bullet* b : Bullet::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255, 0, 0);
	}
	for (RotoText* b : RotoText::GetAll()) {
		b->Draw();
	}
	Window::Draw(Assets::vent, vec(508, 797));
	collider.DebugDraw(0, 255, 0);

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
		ImGui::Text(Mouse::GetPositionInWorld().ToString().c_str());
		ImGui::End();
	}

	//alienPartSys.DrawImGUI();
#endif

	Fx::AfterDraw();
}
