#pragma once

#include "game_data.h"
#include "mates.h"
#include "tilemap.h"
#include "jumpman.h"
#include "scene_manager.h"
#include "transition.h"
#include "bat.h"

struct JumpScene : Scene {

	TransitionSinusFadeOut transition;

	TileMap map;
	JumpMan player;
	PartSys bulletPartSys;

	JumpScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(int dtMilis) override;
	void Draw(sf::RenderTarget& window, bool debug) override;

};
