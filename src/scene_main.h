#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "rototext.h"
#include "net.h"

struct SceneMain : Scene {
	bool is_server;
	TCPsocket server_socket;
	SDLNet_SocketSet server_socket_set;

	Text textTime;
	bool gameover;
	
	float gametime;
	RotoText rotoText;

	SceneMain(bool is_server);

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void SpawnCity();
	void BPS(veci min, veci max, int level);
};
