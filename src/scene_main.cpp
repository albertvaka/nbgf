#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "building.h"
#include "person.h"
#include "waypoint.h"
#include "collide.h"
#include "debug.h"
#include <stack>
#include <vector>

const int STREET_SIZE = 200;
const int GRID_OFFSET = 200;
const int BUILDING_SIZE = 200;

SceneMain::SceneMain(bool is_server) : is_server(is_server) {}

void SceneMain::EnterScene() 
{
	//SpawnBuildings();
	//SpawnWaypoint();
	//SpawnPeople();
	SpawnCity();
}

void AddLinks(Waypoint* p1, Waypoint* p2) {
	p1->AddLink(p2);
	p2->AddLink(p1);
}

void SceneMain::SpawnCity()
{
	constexpr int w = 25;
	constexpr int h = 25;
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

	std::vector< std::vector<Waypoint*> > grid(h, std::vector<Waypoint*>(w));
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			if (maze[x][y] == 'b') {
				new Building(vec(x, y) * STREET_SIZE, vec(BUILDING_SIZE, BUILDING_SIZE));
				grid[x][y] = nullptr;
			}
			else {
				Waypoint* p = new Waypoint(vec(x, y) * STREET_SIZE, STREET_SIZE * 0.25);
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

	for (Waypoint* p : Waypoint::GetAll()) {
		new Person(p->pos);
	}
}

void SceneMain::SpawnWaypoint() {
	std::vector< std::vector<Waypoint*> > grid;
	for (int i = 0; i < 10; i++) {
		std::vector<Waypoint*> aux;
		for (int j = 0; j < 10; j++) {
			Waypoint* w =new Waypoint(
				vec(i*STREET_SIZE+GRID_OFFSET+STREET_SIZE/2, j*STREET_SIZE+GRID_OFFSET+STREET_SIZE/2), 
				STREET_SIZE*0.25
			);
			aux.push_back(w);
		}
		grid.push_back(aux);
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (i > 0) {
				grid[i][j]->AddLink(grid[i-1][j]);
			}
			if (j > 0) {
				grid[i][j]->AddLink(grid[i][j-1]);
			}
			if (j < grid[i].size()-1) {
				grid[i][j]->AddLink(grid[i][j+1]);
			}
			if (i < grid.size()-1) {
				grid[i][j]->AddLink(grid[i+1][j]);
			}
		}
	}	
}


void SceneMain::SpawnBuildings() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			new Building(
				vec(i*STREET_SIZE+GRID_OFFSET, j*STREET_SIZE+GRID_OFFSET), 
				vec(BUILDING_SIZE, BUILDING_SIZE)
			);
		}
	}
}

void SceneMain::SpawnPeople() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			Person* p = new Person(
				vec(i*STREET_SIZE+GRID_OFFSET+STREET_SIZE/2,j*STREET_SIZE+GRID_OFFSET+STREET_SIZE/2)
			);
			for (const Building* b : Building::GetAll()) {
				if (Collide(p, b)) {
					vec away = p->pos - b->pos;
					away.Normalize();
					float awayDistance = sqrt(-p->Bounds().DistanceSq(b->Bounds()))+15;
					p->pos += away*awayDistance;
				}
			}
		}
	}
}

void SceneMain::ExitScene()
{
	Building::DeleteAll();
	Person::DeleteAll();
	Waypoint::DeleteAll();
}

void SceneMain::Update(float dt)
{
	Camera::MoveCameraWithArrows(dt);
	Camera::ChangeZoomWithPlusAndMinus(dt);

	for (auto p : Person::GetAll()) {
		p->Update(dt);
	}

#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		EnterScene();
		return;
	}
#endif
}

void SceneMain::Draw()
{
	Window::Clear(0, 0, 0);

	for (const Building* b : Building::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255,0,0);
	}

	for (const Person* p : Person::GetAll()) {
		p->Draw();
		p->Bounds().DebugDraw(255,0,0);
	}

	for (auto w : Waypoint::GetAll()) {
		w->Draw();
		w->Bounds().DebugDraw(0,255,0);

		if(Debug::Draw) {
			for (auto link : w->links) {
				Window::DrawPrimitive::Arrow(w->pos, link->pos, 2, 10, {0, 255, 0, 255});
			}
		}
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
