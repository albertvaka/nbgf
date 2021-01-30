#pragma once

#include <SDL_net.h>

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"

enum LobbyState {
	IDLE,
	CONNECTING,
	CONNECTED
};

struct OnlinePlayer {
	int id;
	char name[30];
	bool ready;
};

struct SceneMenu : Scene {

	char player_name[30] = "Player";
	bool player_ready = false;
	char ip_text[16] = "127.0.0.1";
	int attempt = 0;
	int client_id = -1;
	LobbyState state = IDLE;
	TCPsocket player_socket;
	SDLNet_SocketSet client_socket_set;
	SceneMenu(bool is_server);
	std::vector<OnlinePlayer> players;

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};
