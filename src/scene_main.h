#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "ship.h"

struct SceneMain : Scene {

	Ship ship;

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
};
