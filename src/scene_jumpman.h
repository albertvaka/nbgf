#pragma once

#include "game_data.h"
#include "mates.h"
#include "tilemap.h"
#include "jumpman.h"
#include "scene_manager.h"
#include "transition.h"
#include "destroyedtiles.h"
#include "bat.h"
#include "powerups.h"
#include "lava.h"
#include "screen.h"

struct JumpScene : Scene {

	TransitionSinusFadeOut transition;
	
	ScreenManager screenManager;

	int randomSeed;
	TileMap map;
	JumpMan player;
	PartSys bulletPartSys;
	DestroyedTiles destroyedTiles;
	GunUp* gunup_tancaporta;

	JumpScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw(sf::RenderTarget& window) override;
	void RandomMap();

};
