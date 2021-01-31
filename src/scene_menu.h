#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "animation.h"

struct SceneMenu : Scene {

	SceneMenu();
	Animation overanim;
	Animation npcanim;

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};
