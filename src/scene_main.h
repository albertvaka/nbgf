#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"
#include "net.h"

struct SceneMain : Scene {
	bool is_server;
	TCPsocket server_socket;
	SDLNet_SocketSet server_socket_set;

	SceneMain(bool is_server);

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void SpawnBuildings();
	void SpawnPeople();
	void SpawnCity();
	void SpawnWaypoint();
	void BPS(veci min, veci max, int level);
};
