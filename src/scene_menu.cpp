#include "scene_menu.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "debug.h"

int MAX_ATTEMPTS = 3;

SceneMenu::SceneMenu()
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
			}
			break;

		case LobbyState::CONNECTING:
			ImGui::Text("Trying to connect...");

			for (int n = 0; n < attempt; n++) {
				player_client_connect
				ImGui::Text("Attempt to connect failed. %d more tries to go...", MAX_ATTEMPTS-n);
			}

			if (ImGui::Button("Cancel")) {
				attempt = 0;
				state = LobbyState::IDLE;
			}

			if (ImGui::Button("Fake Attempt")) {
				attempt += 1;
				if(attempt >= MAX_ATTEMPTS) {
					attempt = 0;
					state = LobbyState::IDLE;
				}
			}

			if (ImGui::Button("Fake Connect"))
			{
				state = LobbyState::CONNECTED;
				OnlinePlayer testPlayer = {
					0,
					"Pepito",
					false
				};
				players.push_back(testPlayer);
			}

			break;

		case LobbyState::CONNECTED:
			ImGui::Begin("Lobby");
			ImGui::Text("The game will start when everyone is ready:");
			ImGui::BeginChild("Scrolling");
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
			ImGui::EndChild();
			if(player_ready) {
				if (ImGui::Button("Ready")) {
					player_ready = true;
				}
			} else {
				if (ImGui::Button("Not ready")) {
					player_ready = false;
				}
			}
			if (ImGui::Button("Disconnect")) {
				state = LobbyState::IDLE;
				players.clear();
			}
			ImGui::End();
			break;
	}
	ImGui::End();
}
