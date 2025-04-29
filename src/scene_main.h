#pragma once

#include "scene.h"
#include "text.h"

struct SceneMain : Scene {

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
};
