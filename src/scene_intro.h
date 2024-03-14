#pragma once

#include "scene.h"
#include "lava.h"
#include "text.h"
#include "parallax.h"

struct IntroScene : Scene
{
	BoxBounds bounds;

	Lava lava;
	Parallax parallax;
	
	Text titleText;
	Text startText;
	Text creditsText;
	
	IntroScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
};
