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

SceneDumb::SceneDumb(bool is_server)
{
	socket = client_connect("127.0.0.1", 8099, &socket_set);
}

void SceneDumb::EnterScene()
{
	Debug::out << "DUMB SCENE";
}

void SceneDumb::ExitScene()
{
	Building::DeleteAll();
}

void SceneDumb::Update(float dt) {
	Debug::out << "UPDATE";
	int num_sockets = SDLNet_CheckSockets(socket_set, 100);
	while (num_sockets <= 0) {
		Debug::out << "Didn't find, checking...";
		num_sockets = SDLNet_CheckSockets(socket_set, 100);
	}

	Debug::out << "Got DATA";

	PACKET_TYPE ptype;
	void *data = recv_data(socket, &ptype);
	Debug::out << "Receive data";

	packet_player_input input;
	int index = 0;
	
	if (Input::IsPressed(0, GameKeys::UP)) {
		input.inputs[index++] = INPUT_ACTION::UP;
	}
	else if (Input::IsPressed(0, GameKeys::DOWN)) {
		input.inputs[index++] = INPUT_ACTION::DOWN;
	}

	if (Input::IsPressed(0, GameKeys::LEFT)) {
		input.inputs[index++] = INPUT_ACTION::LEFT;
	}
	else if (Input::IsPressed(0, GameKeys::RIGHT)) {
		input.inputs[index++] = INPUT_ACTION::RIGHT;
	}

	if (Input::IsJustPressed(0, GameKeys::ACTION)) {
		input.inputs[index++] = INPUT_ACTION::ACTION;
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
	}

}
