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
			new Building(vec(i*100+200, j*100+200), vec(30, 30));
		}
	}
}

void SceneMain::SpawnPeople() {
	new Person(vec(0,0));
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

	Window::Draw(Assets::backgroundTexture, Camera::Center())
		.withOrigin(Assets::backgroundTexture->w/2, Assets::backgroundTexture->h/2);


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
