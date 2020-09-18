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
#include "rototext.h"

struct Lava;
struct GunUp;
struct EnemyDoor;

struct JumpScene : Scene {

	ScreenManager screenManager;

	TileMap map;
	JumpMan player;
	SkillTree skillTree;
	PartSys fogPartSys;
	DestroyedTiles destroyedTiles;
	RotoText rotoText;

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
	vec GetCameraTargetPos();
	void UpdateCamera(float dt);

};
