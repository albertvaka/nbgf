#include "scene_main.h"
#include <random>
#include <algorithm>
#include "raw_input.h"
#include "scene_manager.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "building.h"
#include "person.h"
#include "arrow.h"
#include "waypoint.h"
#include "overlord.h"
#include "collide.h"
#include "debug.h"
#include "freeze_skill.h"
#include "wave_skill.h"
#include <stack>
#include "musicplayer.h"
#include <vector>

const int NUM_GOALS = 4;

const int STREET_SIZE = 200;
const int GRID_OFFSET = 200;
const int BUILDING_SIZE = 200;
const int WAYPOINT_SIZE = STREET_SIZE * 0.3;

constexpr int bsp_levels = 4;
constexpr float bsp_margin_ratio = 0.2f;

constexpr int w = 18;
constexpr int h = 10;

SceneMain::SceneMain()
	: textTime(Assets::font_30)
	, rotoText(Assets::font_30, Assets::font_30_outline)
	, close_eyes_text(Assets::font_120, Assets::font_120_outline)
{
	close_eyes_text.SetOutlineColor(255, 0, 0);
	MusicPlayer::PlayWithIntro(Assets::music, Assets::music_intro);

	Camera::SetZoom(0.425f);
	Camera::SetTopLeft(vec(20, -80));
}

void SceneMain::EnterScene() {
	SpawnCity();
	curr_stage = OVERSEER_CLOSE_EYES;
	gameover = false;
	rotoText.timer = -1;
}

void AddLinks(Waypoint* p1, Waypoint* p2) {
	p1->AddLink(p2);
	p2->AddLink(p1);
}

void BSP(std::array<std::array<char,h>,w>& maze, veci min, veci max, int level) {
	if (level == 0) return;
	int width = max.x - min.x;
	int height = max.y - min.y;
	if (width > height)
	{
		int margin = ceilf((max.x - min.x) * bsp_margin_ratio);
		if (min.x + margin >= max.x - margin) return;
		int x = Rand::roll(min.x + margin, max.x - margin);
		for (int y = min.y; y < max.y; y++) {
			maze[x][y] = 'e';
		}
		BSP(maze, min, veci(x, max.y), level - 1);
		BSP(maze, veci(x, min.y), max, level - 1);
	}
	else 
	{
		int margin = ceilf((max.y - min.y)* bsp_margin_ratio);
		if (min.y + margin >= max.y - margin) return;
		int y = Rand::roll(min.y+margin, max.y-margin);
		for (int x = min.x; x < max.x; x++) {
			maze[x][y] = 'e';
		}
		BSP(maze, min, veci(max.x, y), level - 1);
		BSP(maze, veci(min.x, y), max, level - 1);
	}
}

void SceneMain::SpawnCity()
{
	std::array < std::array<char, h>, w> maze;
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			maze[x][y] = 'b';
		}
	}

	for (int y = 1; y < h-1; y++) {
		maze[1][y] = 'e';
		maze[w-2][y] = 'e';
	}

	for (int x = 1; x < w-1; x++) {
		maze[x][1] = 'e';
		maze[x][h-2] = 'e';
	}

	BSP(maze, veci(1, 1), veci(w - 1, h - 1), bsp_levels);

	std::vector< std::vector<Waypoint*> > grid(w, std::vector<Waypoint*>(h));
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			if (maze[x][y] == 'b') {
				if (y < h-1 && maze[x][y + 1] == 'b' && Rand::OnceEvery(2)) {
					//1x2
					new Building(
						Assets::buildings1x2[Rand::roll(Assets::buildings1x2.size())],
						vec(x, y) * STREET_SIZE + vec(0, BUILDING_SIZE / 2),
						vec(BUILDING_SIZE, BUILDING_SIZE * 2)
					);
					maze[x][y + 1] = 'm';
				} else if (x < w-1 && maze[x + 1][y] == 'b' && Rand::OnceEvery(2)) {
					//2x1
					new Building(
						Assets::buildings2x1[Rand::roll(Assets::buildings2x1.size())],
						vec(x, y) * STREET_SIZE + vec(BUILDING_SIZE/2,0),
						vec(BUILDING_SIZE*2, BUILDING_SIZE)
					);
					maze[x + 1][y] = 'm';
				}
				else 
				{
					//1x1
					new Building(
						Assets::buildings1x1[Rand::roll(Assets::buildings1x1.size())],
						vec(x, y) * STREET_SIZE, 
						vec(BUILDING_SIZE, BUILDING_SIZE)
					);
				}
				grid[x][y] = nullptr;
			}
			else if (maze[x][y] == 'e') {
				Waypoint* p = new Waypoint(vec(x, y) * STREET_SIZE, WAYPOINT_SIZE);
				grid[x][y] = p;
				if (grid[x - 1][y]) {
					AddLinks(p, grid[x - 1][y]);
				}
				if (grid[x][y - 1]) {
					AddLinks(p, grid[x][y - 1]);
				}
			}
		}
	}

	std::vector<Waypoint*> empty_wp;
	for (Waypoint* p : Waypoint::GetAll()) {
		if (Rand::PercentChance(35)) {
			new Person(p->pos, -1);
		}
		else {
			empty_wp.push_back(p);
		}
	}

	std::shuffle(empty_wp.begin(), empty_wp.end(), GoodRand::r.gen);
	int num_players = GamePad::ConnectedGamepads();
	if (num_players == 0) {
		num_players = 1;
	}
	int player_id = 0;
	for (Waypoint* p : empty_wp) {
		Person *per = new Person(p->pos, player_id++);
		if (player_id <= num_players) {
			new Arrow(per);
		}

		if (player_id == num_players) break;
	}


	new Overlord();
}

void SceneMain::ExitScene()
{
	Building::DeleteAll();
	Person::DeleteAll();
	Waypoint::DeleteAll();
	Overlord::DeleteAll();
}

void SceneMain::Update(float dt)
{
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		SceneManager::RestartScene();
		return;
	}

	if (gameover) {
		rotoText.Update(dt);
		return;
	}

	close_eyes_text.SetString("OVERSEER CLOSE\nYOUR EYES");
	close_eyes_text.SetMultilineAlignment(Text::MultilineAlignment::CENTER);

	time_until_next_stage -= dt;
	if (curr_stage == OVERSEER_CLOSE_EYES) {
		if (time_until_next_stage <= 0.0f) {
			curr_stage = SHOW_ARROWS;
			time_until_next_stage = stage_duration;
		}
	}
	else if (curr_stage == SHOW_ARROWS) {
		if (time_until_next_stage <= 0.0f) {
			curr_stage = GAME;
			time_until_next_stage = stage_duration;
		}
	}


	bool playersAlive = false;
	for (auto p : Person::GetAll()) {
		if (p->player_id >= 0) {
			p->UpdatePlayer(dt);
			if (p->alive) {
				playersAlive = true;
			}
		} else {
			p->UpdateNpc(dt);
		}
	}

	for (auto a : Arrow::GetAll()) {
		a->Update(dt);
	}

	for (auto o : Overlord::GetAll()) {
		o->Update(dt);
	}

	for (auto o : FreezeSkill::GetAll()) {
		o->Update(dt);
	}
	FreezeSkill::DeleteNotAlive();

	for (auto o : WaveSkill::GetAll()) {
		o->Update(dt);
	}
	WaveSkill::DeleteNotAlive();

	int goalsdone = 0;
	/*for (auto o : Goal::GetAll()) {
		o->Update(dt);
	}
	*/
	if (goalsdone  == NUM_GOALS || !playersAlive) {
		gameover = true;
		rotoText.ShowMessage(playersAlive?"The dissidents\ngot away" : "The regime\ntriumphs");
		WaveSkill::DeleteAll();
		FreezeSkill::DeleteAll();
	}
	textTime.SetString("Bombs planted: "+ std::to_string(goalsdone) + "/" + std::to_string(NUM_GOALS));

}

std::vector<Window::PartialDraw> draws;
std::vector<Window::PartialDraw*> drawps;
void SceneMain::Draw()
{
	Window::Clear(0, 5, 20);

	draws.clear();

	for (const Building* b : Building::GetAll()) {
		draws.push_back(b->Draw());
		b->Bounds().DebugDraw(255,0,0);
	}

	for (const Person* p : Person::GetAll()) {
		draws.push_back(p->Draw());
		p->Bounds().DebugDraw(255,0,0);
	}

	if (curr_stage == SHOW_ARROWS) {
		for (const Arrow* a : Arrow::GetAll()) {
			draws.push_back(a->Draw());
		}
	}

	for (auto w : Waypoint::GetAll()) {
		w->Bounds().DebugDraw(0,255,0);

		if(Debug::Draw) {
			for (auto link : w->links) {
				Window::DrawPrimitive::Arrow(w->pos, link->pos, 2, 10, {0, 255, 0, 255});
			}
		}
	}

	drawps.clear();
	for (Window::PartialDraw& pd : draws) {
		drawps.push_back(&pd);
	}

	std::sort(drawps.begin(), drawps.end(), [](Window::PartialDraw* a, Window::PartialDraw* b) {
		return (a->dest.y) < (b->dest.y);
	});

	for (Window::PartialDraw* pd : drawps) {
		pd->DoDraw();
	}

	for (const WaveSkill* b : WaveSkill::GetAll()) {
		b->Draw();
	}

	// Has to be last because contains GUI
	for (auto o : Overlord::GetAll()) {
		o->Draw();
	}
	for (auto o : FreezeSkill::GetAll()) {
		o->Draw();
	}

	if (curr_stage == OVERSEER_CLOSE_EYES) {
		Window::Draw(close_eyes_text, Camera::Center())
			.withOrigin(close_eyes_text.Size()/2);
	}

	Camera::InScreenCoords::Begin();

	Window::Draw(textTime, vec(5, 5))
		.withOrigin(0, 0)
		.withScale(0.5f);

	Camera::InScreenCoords::End();

	rotoText.Draw();

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::End();
	}
#endif

}
