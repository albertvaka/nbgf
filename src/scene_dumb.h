#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "net.h"

struct SceneDumb : Scene {
	TCPsocket socket;
	SDLNet_SocketSet socket_set;

	SceneDumb(bool is_server);

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
};
