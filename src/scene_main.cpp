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
#include "waypoint.h"
#include "overlord.h"
#include "collide.h"
#include "debug.h"
#include "musicplayer.h"
#include <vector>

const int STREET_SIZE = 200;
const int GRID_OFFSET = 200;
const int BUILDING_SIZE = 200;
const int WAYPOINT_SIZE = STREET_SIZE * 0.3;

constexpr int bsp_levels = 4;

constexpr int w = 19;
constexpr int h = 11;

SceneMain::SceneMain(bool is_server) : is_server(is_server) {
	MusicPlayer::PlayWithIntro(Assets::music, Assets::music_intro);

	Camera::SetZoom(0.4);
	Camera::SetTopLeft(vec(0, 0));
}

void SceneMain::EnterScene() {
	if (is_server) {
		server_socket = server_setup(8099, &server_socket_set);
	}

	SpawnCity();
}

void AddLinks(Waypoint* p1, Waypoint* p2) {
	p1->AddLink(p2);
	p2->AddLink(p1);
}

void BSP(std::array<std::array<char,h>,w>& maze, veci min, veci max, int level) {
	if (level == 0) return;
	int width = max.x - min.x;
	int height = max.y - min.y;
	float marginratio = 0.3f;
	if (width > height)
	{
		int margin = ceilf((max.x - min.x) * marginratio);
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
		int margin = ceilf((max.y - min.y)* marginratio);
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
		if (Rand::OnceEvery(2)) {
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
		new Person(p->pos, player_id++);
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

std::vector<EntityUpdate> entities;
void SceneMain::Update(float dt)
{
	
	std::map<int, packet_player_input> inputs;
	for (Client& client : lobby.clients) {
		if (!client.in_use) continue;
		PACKET_TYPE type;
		void* data = recv_data(client.socket, &type);
		if (type == PACKET_TYPE::PLAYER_INPUT) {
			packet_player_input input = parse_player_input(data);
			inputs[input.client_id] = input;
		}
	}

	for (auto p : Person::GetAll()) {
		if (p->player_id >= 0) {
			p->UpdatePlayer(dt);
		} else {
			p->UpdateNpc(dt);
		}
	}

	// TODO: network stuff
	for (auto o : Overlord::GetAll()) {
		o->Update(dt);
	}

#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		SceneManager::RestartScene();
		return;
	}
#endif

	/*
	entities.clear();
	for (Person* p : Person::GetAll()) {
		entities.push_back(p->Serialize());
	}

	for (Client& client : lobby.clients) {
		if (!client.in_use) continue;
		send_entity_data(client.socket, &entities[0], entities.size());
	}*/
}

std::vector<Window::PartialDraw> draws;
std::vector<Window::PartialDraw*> drawps;
void SceneMain::Draw()
{
	Window::Clear(1, 10, 33);

	draws.clear();
	for (const Building* b : Building::GetAll()) {
		draws.push_back(b->Draw());
		b->Bounds().DebugDraw(255,0,0);
	}

	for (const Person* p : Person::GetAll()) {
		draws.push_back(p->Draw());
		p->Bounds().DebugDraw(255,0,0);
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

	// Has to be last because contains GUI
	for (auto o : Overlord::GetAll()) {
		o->Draw();
	}
#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::End();
	}
#endif

}
