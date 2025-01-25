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
vec playerTextPos(550, 150);
float rotoArea = 120;
BoxBounds collider = BoxBounds(400, 287, 800, 20);
float rotoScale = 0.85f;

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
	: alienPartSys(Assets::spritesheetTexture)
	, scoreText { Text(Assets::funk_30, Assets::funk_30_outline),Text(Assets::funk_30, Assets::funk_30_outline) }
{

	notePlaying[0][0].sound = &Assets::note1p1;
	notePlaying[0][1].sound = &Assets::note2p1;
	notePlaying[0][2].sound = &Assets::note3p1;
	notePlaying[0][3].sound = &Assets::note4p1;
	notePlaying[1][0].sound = &Assets::note1p2;
	notePlaying[1][1].sound = &Assets::note2p2;
	notePlaying[1][2].sound = &Assets::note3p2;
	notePlaying[1][3].sound = &Assets::note4p2;

	scoreText[0].SetOutlineColor(0, 0, 0);
	scoreText[0].SetFillColor(255, 255, 255);

	scoreText[1].SetOutlineColor(0, 0, 0);
	scoreText[1].SetFillColor(255, 255, 255);

	alienPartSys.AddSprite({ 16 * 16,17 * 16,16, 16 });
	alienPartSys.min_interval = 0.3f;
	alienPartSys.max_interval = 1.0f;
	alienPartSys.min_ttl = 5.0f;
	alienPartSys.max_ttl = 5.0f;
	alienPartSys.min_scale = 1.0f;
	alienPartSys.max_scale = 1.6f;
	alienPartSys.max_vel = vec(0,-250);
	alienPartSys.min_vel = vec(0,-450);
	alienPartSys.min_rotation = 0;
	alienPartSys.max_rotation = 360;
	alienPartSys.max_rotation_vel = 100.f;
	alienPartSys.min_rotation_vel = 100.f;
	alienPartSys.alpha = 0.45f;
	alienPartSys.alpha_vel = -0.4f;
	alienPartSys.pos.y = Window::GAME_HEIGHT;
	LoadSong();

}

void SceneMain::EnterScene() 
{
	Fx::BeforeEnterScene();
	Fx::FullscreenShader::SetShader([]() {
		Assets::underwaterShader.Activate();
		Assets::underwaterShader.SetUniform("time", mainClock*4);
	});

	MusicPlayer::SetVolume(20);
	MusicPlayer::Play(Assets::menuMusic);
	currentTime = 0;
	currentNote = 0;
	score[0] = 0;
	score[1] = 0;
	combo[0] = 0;
	combo[1] = 0;
	updateScore(0);
	updateScore(1);
}

void SceneMain::ExitScene()
{
	Bullet::DeleteAll();
	RotoText::DeleteAll();
}

void SceneMain::updateScore(int player) {
	// TODO: Combo increase animation
	scoreText[player].SetString("Score: " + std::to_string(score[player]) + "\n\nCombo: x" + std::to_string(combo[player]));
}

void playerFloatingText(int player, std::string text) {
	vec pos = playerTextPos;
	if (player == 1) {
		pos.x = Window::GAME_WIDTH - pos.x;
	}
	auto roto = new RotoText(pos + Rand::PosInsideCircle(rotoArea), Assets::funk_30, Assets::funk_30_outline);
	roto->ShowMessage(text);
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

	for (float x = 610; x < 1060; x += 15.f) {
		alienPartSys.pos.x = x;
		alienPartSys.Spawn(dt);
	}
	alienPartSys.UpdateParticles(dt);

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
		if (b->Bounds().Bottom() < collider.Top()) {
			b->alive = false;
			if (!b->hit[0]) {
				combo[0] = 0;
				playerFloatingText(0, "Miss");
				updateScore(0);
			}
			if (!b->hit[1]) {
				combo[1] = 0;
				playerFloatingText(1, "Miss");
				updateScore(1);
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
	}
	if (collide) {
		combo[player]++;
		switch (Rand::roll(3)) {
			case 0: playerFloatingText(player, "yo!"); break;
			case 1: playerFloatingText(player, "c'mon"); break;
			case 2: playerFloatingText(player, "yeah"); break;
			default: playerFloatingText(player, "Yo!!!!!!!"); break;
		}
		score[player] += 100 * sqrt(combo[player]);
		updateScore(player);
	}
	else {
		combo[player] = 0;
		playerFloatingText(player, "Oops!");
	}

}

float arg1 = 855;
float arg2 = 1370;
float arg3 = 150;
float arg4 = 2;

float triangle(float x) {
	return acos(sin(x)) / 1.5708;
}

void SceneMain::Draw()
{
	Fx::FullscreenShader::Begin();
	Window::Clear(100, 100, 230, 255);
	Window::Draw(Assets::seaBgTexture, vec::Zero);
	Window::Draw(Assets::air, vec(715, 0)).withRect(0, mainClock*500.f, Assets::air->w, Window::GAME_HEIGHT);
	alienPartSys.Draw();
	Window::Draw(Assets::window, vec(Window::GAME_WIDTH / 2, 280)).withOriginCentered();

	float fishScaleX = 0.25f;
	float fishScaleY = 0.25f + sin(2*mainClock*Angles::Pi) / 100;
	Window::Draw(Assets::fish1mic, vec(280, 830 - 2000 * fishScaleY))
		.withScale(0.25f * 1.1, fishScaleY*1.1);
	Window::Draw(Assets::fish1, vec(17, 211+ Assets::fish1->h*0.25)).withOrigin(0,Assets::fish1->h).withScale(fishScaleX, fishScaleY);
	Window::Draw(Assets::fish1mouth, vec(314, 932 + fishScaleY * Assets::fish1mouth->h - 2420 * fishScaleY))
		.withOrigin(0, Assets::fish1mouth->h)
		.withScale(fishScaleX, fishScaleY*0.9 - 0.07 * abs(sin(mainClock * 6.2 - 2)));
	Window::Draw(Assets::fish1arm, vec(175, 950 - 2000 * fishScaleY))
		.withOrigin(30, 25)
		.withScale(fishScaleX, fishScaleY)
		.withRotationDegs(15*triangle(Angles::Pi/2 + 2*mainClock * Angles::Pi));

	for (const Bullet* b : Bullet::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255, 0, 0);
	}

	for (RotoText* b : RotoText::GetAll()) {
		b->Draw(rotoScale);
	}

	Window::Draw(Assets::vent, vec(508, 797));
	collider.DebugDraw(0, 255, 0);

	Fx::FullscreenShader::End();

	//Draw GUI
	Camera::InScreenCoords::Begin();
	Window::Draw(scoreText[0], vec(270, 56))
		.withOrigin(scoreText[0].Size() / 2);
	Window::Draw(scoreText[1], vec(Window::GAME_WIDTH - 270, 56))
		.withOrigin(scoreText[1].Size() / 2);

	Camera::InScreenCoords::End();

#ifdef _IMGUI
	{
		alienPartSys.DrawImGUI();
		ImGui::Begin("scene");
		ImGui::Text(Mouse::GetPositionInWorld().ToString().c_str());
		ImGui::SliderFloat("Arg1", &arg1, 0, 2000);
		ImGui::SliderFloat("Arg2", &arg2, 500, 3000);
		ImGui::SliderFloat("Arg3", &arg3, 0, 1000);
		ImGui::SliderFloat("Arg4", &arg4, 0, 8);
		ImGui::End();
	}

#endif
	Fx::AfterDraw();
}
