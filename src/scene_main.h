#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"
#include "gaemtilemap.h"

struct SceneMain : Scene {

	Player player;
	GaemTileMap map;
	Text deadAliensText;

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};
