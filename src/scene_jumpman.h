#pragma once

#include "mates.h"
#include "tilemap.h"
#include "jumpman.h"
#include "scene_manager.h"
#include "destroyedtiles.h"
#include "powerups.h"
#include "skilltree.h"
#include "screen.h"
#include "lava.h"
#include "enemy_door.h"
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
	EnemyDoor* gunup_tancaporta_door;
	Lava* raising_lava;
	float raising_lava_target_height;

	GameKeys contextActionButton = GameKeys::NONE;

	JumpScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
	void RandomMap();

};
