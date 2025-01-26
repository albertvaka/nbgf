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
#include "input.h"
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
float sceneClock = 0;

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
	, scoreText{ Text(Assets::funk_30, Assets::funk_30_outline),Text(Assets::funk_30, Assets::funk_30_outline) }
	, comboText{ Text(Assets::funk_30, Assets::funk_30_outline),Text(Assets::funk_30, Assets::funk_30_outline) }
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

	comboText[0].SetOutlineColor(0, 0, 0);
	comboText[0].SetFillColor(255, 255, 255);
	comboText[1].SetOutlineColor(0, 0, 0);
	comboText[1].SetFillColor(255, 255, 255);

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
	sceneClock = 0;

	Fx::ScreenTransition::Start(Assets::fadeInDiamondsShader);
}

void SceneMain::ExitScene()
{
	Bullet::DeleteAll();
	RotoText::DeleteAll();
	OneShotAnim::DeleteAll();
}

void SceneMain::updateScore(int player) {
	scoreText[player].SetString("Score: " + std::to_string(score[player]));
	comboText[player].SetString("Combo: x" + std::to_string(combo[player]));
}

void playerFloatingText(int player, std::string text, bool bad = false) {
	vec pos = playerTextPos;
	if (player == 1) {
		pos.x = Window::GAME_WIDTH - pos.x;
	}
	auto roto = new RotoText(pos + Rand::PosInsideCircle(rotoArea), Assets::funk_30, Assets::funk_30_outline);
	roto->ShowMessage(text);
	if (bad) {	
		roto->SetFillColor(250, 50, 50);
	}
	else {
		roto->SetFillColor(50, 255, 50);
	}
}


void SceneMain::Update(float dt)
{
	Fx::Update(dt);

#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F6;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		return;
	}
#endif

	sceneClock += dt;

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
	for (OneShotAnim* b : OneShotAnim::GetAll()) {
		b->Update(dt);
	}
	OneShotAnim::DeleteNotAlive();
	for (RotoText* b : RotoText::GetAll()) {
		b->Update(dt);
	}
	RotoText::DeleteNotAlive();
	for (Bullet* b : Bullet::GetAll()) {
		b->Update(dt);
		b->active = Collide(collider, b->Bounds());
		if (b->Bounds().Bottom() < collider.Top()) {
			b->alive = false;
			b->Explode();
			if (!b->hit[0]) {
				combo[0] = 0;
				playerFloatingText(0, "Miss", true);
				updateScore(0);
			}
			if (!b->hit[1]) {
				combo[1] = 0;
				playerFloatingText(1, "Miss", true);
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
		playerFloatingText(player, "Oops!", true);
	}

}

float arg1 = 1470;
float arg2 = 920;
float arg3 = 2900;
float arg4 = -10.f;

float triangle(float x) {
	return acos(sin(x)) / 1.5708;
}

void SceneMain::Draw()
{
	Window::Clear(255, 255, 255, 255);
	Fx::FullscreenShader::Begin();
	Window::Clear(100, 100, 230, 255);
	Window::Draw(Assets::seaBgTexture, vec::Zero);
	Window::Draw(Assets::air, vec(715, 0)).withRect(0, mainClock*500.f, Assets::air->w, Window::GAME_HEIGHT);
	alienPartSys.Draw();
	Window::Draw(Assets::window, vec(Window::GAME_WIDTH / 2, 280)).withOriginCentered();

	// FISH 1
	float flow = 2 * sceneClock * Angles::Pi;
	{
		float fishScaleX = 0.25f;
		float baseFishScaleY = 0.25f;
		float fishScaleY = baseFishScaleY  + sin(flow) / 100;
		Window::Draw(Assets::fish1mic, vec(300, 820 - 2000 * fishScaleY))
			.withScale(fishScaleX * 1.1, fishScaleY * 1.1)
			.withRotationDegs(5 * cos(flow));
		Window::Draw(Assets::fish1, vec(37, 211 + Assets::fish1->h * baseFishScaleY))
			.withOrigin(0, Assets::fish1->h)
			.withScale(fishScaleX, fishScaleY);
		Window::Draw(Assets::fish1mouth, vec(334, 932 + fishScaleY * Assets::fish1mouth->h - 2420 * fishScaleY))
			.withOrigin(0, Assets::fish1mouth->h)
			.withScale(fishScaleX, fishScaleY * 0.9 - 0.07 * abs(sin(mainClock * 6.2 - 2)));
		Window::Draw(Assets::fish1arm, vec(195, 950 - 2000 * fishScaleY))
			.withOrigin(30, 25)
			.withScale(fishScaleX, fishScaleY)
			.withRotationDegs(15 * triangle(Angles::Pi / 2 + flow));
	}

	// FISH 2
	{
		float fish2ScaleX = 0.155f;
		float baseFish2ScaleY = 0.155f;
		float fish2ScaleY = baseFish2ScaleY  + sin(Angles::Pi + flow) / 160;
		Window::Draw(Assets::fish2mic, vec(1200, 950 - 4000 * fish2ScaleY -8 * sin(Angles::Pi /2 + Angles::Pi + flow)))
			.withScale(fish2ScaleX, fish2ScaleY);
		Window::Draw(Assets::fish2mouth, vec(1431, 870 + fish2ScaleY * Assets::fish2mouth->h - 4000 * fish2ScaleY))
			.withOrigin(Assets::fish2mouth->w - 120, 0)
			.withScale(fish2ScaleX * 0.75, fish2ScaleY * 0.75)
			.withRotationDegs(10 * sin(Angles::Pi + flow));
		Window::Draw(Assets::fish2, vec(1360, 200 + Assets::fish2->h * baseFish2ScaleY))
			.withOrigin(0, Assets::fish2->h)
			.withScale(fish2ScaleX, fish2ScaleY);
		Window::Draw(Assets::fish2arm, vec(1470, 920 - 2900 * fish2ScaleY))
			.withOrigin(1430, 190)
			.withScale(fish2ScaleX*1.1, fish2ScaleY)
			.withRotationDegs(-10 + 15 * triangle(Angles::Pi / 2 + flow));
	}

	for (const Bullet* b : Bullet::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255, 0, 0);
	}
	for (const OneShotAnim* b : OneShotAnim::GetAll()) {
		b->Draw();
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
	{
		int rotationNumber = combo[0];
		float scaleNumber = 0;
		if (combo[0] > 15) {
			rotationNumber = 20;
			scaleNumber = combo[0] - 10;
		}
		Window::Draw(comboText[0], vec(270, 106))
			.withRotationDegs(10 * sin(mainClock * rotationNumber))
			.withScale(1 + 0.5 * abs(sin(mainClock * scaleNumber)))
			.withOrigin(comboText[0].Size() / 2);
	}
	{
		int rotationNumber = combo[1];
		float scaleNumber = 0;
		if (combo[1] > 15) {
			rotationNumber = 20;
			scaleNumber = combo[1] - 10;
		}
		Window::Draw(comboText[1], vec(Window::GAME_WIDTH - 270, 106))
			.withRotationDegs(10 * sin(mainClock * rotationNumber))
			.withScale(1 + 0.5 * abs(sin(mainClock * scaleNumber)))
			.withOrigin(comboText[1].Size() / 2);
	}

	Camera::InScreenCoords::End();

#ifdef _IMGUI
	{
		alienPartSys.DrawImGUI();
		ImGui::Begin("scene");
		ImGui::Text(Mouse::GetPositionInWorld().ToString().c_str());
		ImGui::SliderInt("Arg1", &(combo[1]), 0, 50);
		ImGui::SliderFloat("Arg2", &arg2, 0, 3000);
		ImGui::SliderFloat("Arg3", &arg3, 0, 5000);
		ImGui::SliderFloat("Arg4", &arg4, -30.f, 30.f);
		ImGui::End();
	}

#endif
	Fx::AfterDraw();
}
