#pragma once

#include "tilemap.h"
#include "jumpman.h"
#include "scene.h"
#include "destroyedtiles.h"
#include "skilltree.h"
#include "screen.h"
#include "input.h"
#include "rototext.h"

struct Lava;
struct GunUp;
struct EnemyDoor;
struct BigItem;

struct JumpScene : Scene {

	ScreenManager screenManager;

	TileMap map;
	JumpMan player;
	SkillTree skillTree;
	PartSys fogPartSys;
	DestroyedTiles destroyedTiles;
	RotoText rotoText;
	Entity* boss_bipedal;
	int saveSlot;

	EnemyDoor* door_to_close_when_break_skill;

	Lava* raising_lava;
	float raising_lava_target_height;

	bool contextActionButton = false;

	void SaveGame() const;
	void LoadGame();

	JumpScene(int saveSlot = 0);
	~JumpScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
	void UpdateCamera(float dt);


	void TriggerPickupItem(BigItem * g, bool fromSave);
};
