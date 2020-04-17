#pragma once

#include "game_data.h"
#include "mates.h"
#include "tilemap.h"
#include "jumpman.h"
#include "scene_manager.h"
#include "transition.h"
#include "destroyedtiles.h"
#include "powerups.h"
#include "screen.h"
#include "input.h"

struct JumpScene : Scene {

	TransitionSinusFadeOut transition;
	
	ScreenManager screenManager;

	int randomSeed;
	TileMap map;
	JumpMan player;
	PartSys bulletPartSys;
	PartSys fogPartSys;
	DestroyedTiles destroyedTiles;
	GunUp* gunup_tancaporta;

	GameKeys contextActionButton = GameKeys::NONE;

	JumpScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw(sf::RenderTarget& window) override;
	void RandomMap();

};
