#pragma once

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
	char ip_text[16] = "0.0.0.0";
	int attempt = 0;
	LobbyState state = IDLE;
	SceneMenu();
	std::vector<OnlinePlayer> players;

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};


