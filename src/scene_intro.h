#pragma once

#include "scene.h"
#include "text.h"

struct SceneIntro : Scene {

	Text titleText;
	Text startText;
	Text creditsText;

	SceneIntro();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
};
