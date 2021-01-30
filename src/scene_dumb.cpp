#include "scene_dumb.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "building.h"
#include "person.h"
#include "net.h"
#include "waypoint.h"
#include "collide.h"
#include "debug.h"
#include <stack>
#include <vector>

SceneDumb::SceneDumb() {
	
}

void SceneDumb::EnterScene()
{

}

void SceneDumb::ExitScene()
{
	Building::DeleteAll();
}

void SceneDumb::Update(float dt)
{
	packet_player_input input;
	int index = 0;
	
	if (Input::IsPressed(0, GameKeys::UP)) {
		input.inputs[index++] = INPUT::UP;
	}
	else if (Input::IsPressed(0, GameKeys::DOWN)) {
		input.inputs[index++] = INPUT::DOWN;
	}

	if (Input::IsPressed(0, GameKeys::LEFT)) {
		input.inputs[index++] = INPUT::LEFT;
	}
	else if (Input::IsPressed(0, GameKeys::RIGHT)) {
		input.inputs[index++] = INPUT::RIGHT;
	}

	if (Input::IsJustPressed(0, GameKeys::ACTION)) {
		input.inputs[index++] = INPUT::ACTION;
	}

	//send_data
}

void SceneDumb::Draw()
{
	Window::Clear(0, 0, 0);


	for (const Building* b : Building::GetAll()) {
		b->Draw();
		b->Bounds().DebugDraw(255,0,0);
	}

	int num_entities = -1;
	for (int i = 0; i < num_entities;  i++) {
		EntityUpdate* entity =nullptr;
		Person::DumbDraw(entity);
	}

}
