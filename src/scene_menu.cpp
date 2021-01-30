#include "scene_menu.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "debug.h"
#include "net.h"

int MAX_ATTEMPTS = 3;

SceneMenu::SceneMenu(bool is_server)
{
}

void SceneMenu::EnterScene() 
{
}


void SceneMenu::ExitScene()
{
}

void SceneMenu::Update(float dt)
{

#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		EnterScene();
		return;
	}
#endif
}

void SceneMenu::Draw() {
	Window::Clear();
	ImGui::Begin("Connect");
	switch(state) {
		case LobbyState::IDLE:
			ImGui::InputText(" Name", player_name, IM_ARRAYSIZE(player_name));
			ImGui::InputText(" IP", ip_text, IM_ARRAYSIZE(ip_text));
			if (ImGui::Button("Connect"))
			{
				Debug::out << "Connecting to..." << ip_text;
				state = CONNECTING;
				player_socket = client_connect(ip_text, 8099, &client_socket_set);
			}
			break;

		case LobbyState::CONNECTING:
			ImGui::Text("Trying to connect...");

			if (attempt == 0) {
				player_socket = client_connect(ip_text, 8099, &client_socket_set);
				send_join_lobby(player_socket);
				PACKET_TYPE ptype;
				void* data = recv_data(player_socket, &ptype);

				if (ptype != JOIN_LOBBY_RESPONSE) {
					Debug::out << "Expected a JOIN LOBBY RESPONSE";
				}
				else {
					packet_join_lobby_response pjlr = parse_join_lobby_response(data);
					client_id = pjlr.client_id;
				}

				attempt += 1;
			}

			if (player_socket == NULL) {
				ImGui::Text("CONNECTION FAILED");
			}
			else {
				state = LobbyState::CONNECTED;
			}

			break;

		case LobbyState::CONNECTED:
			ImGui::Text("The game will start when everyone is ready:");
			if (player_ready) {
				ImGui::Text("%s (You) - Ready", player_name);
			} else {
				ImGui::Text("%s (You) - Not Ready", player_name);
			}

			for (int n = 0; n < players.size(); n++) {
				if (players[n].ready) {
					ImGui::Text("%s - Ready", players[n].name);
				} else {
					ImGui::Text("%s - Not Ready", players[n].name);
				}
			}

			if(player_ready) {
				if (ImGui::Button("Ready")) {
					player_ready = false;
					send_player_ready(player_socket, client_id, player_ready);
				}
			} else {
				if (ImGui::Button("Not ready")) {
					player_ready = true;
				}
			}

			if (ImGui::Button("Disconnect")) {
				state = LobbyState::IDLE;
				players.clear();
			}

			break;
	}
	ImGui::End();
}
