#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"

struct SceneDumb : Scene {

	SceneDumb(bool is_server);

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};


