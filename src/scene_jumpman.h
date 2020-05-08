#pragma once

#include "mates.h"
#include "tilemap.h"
#include "jumpman.h"
#include "scene_manager.h"
#include "destroyedtiles.h"
#include "powerups.h"
#include "skilltree.h"
#include "screen.h"
#include "input.h"

struct JumpScene : Scene {

	ScreenManager screenManager;

	int randomSeed;
	TileMap map;
	JumpMan player;
	SkillTree skillTree;
	PartSys bulletPartSys;
	PartSys fogPartSys;
	DestroyedTiles destroyedTiles;
	GunUp* gunup_tancaporta;

	GameKeys contextActionButton = GameKeys::NONE;

	JumpScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
	void RandomMap();

};
