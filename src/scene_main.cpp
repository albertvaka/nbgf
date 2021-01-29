#include "scene_main.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "building.h"
#include "person.h"
#include "collide.h"
#include "debug.h"

SceneMain::SceneMain() {
	
}

void SceneMain::EnterScene() 
{
	SpawnBuildings();
	SpawnPeople();
}

void SceneMain::SpawnBuildings() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			new Building(vec(i*400+200, j*400+200), vec(200, 200));
		}
	}
}

void SceneMain::SpawnPeople() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			Person* p = new Person(vec(i*31+200,j*31+200));
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
}

void SceneMain::Update(float dt)
{

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

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::End();
	}
#endif

}
