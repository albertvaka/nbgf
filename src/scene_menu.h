#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"

struct SceneMenu : Scene {

	SceneMenu();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};
