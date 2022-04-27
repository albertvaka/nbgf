#pragma once

#include "gaemtilemap.h"
#include "jumpman.h"
#include "scene.h"
#include "destroyedtiles.h"
#include "skilltree.h"
#include "rototext.h"
#include "zonemanager.h"
#include "dialogdriver.h"

struct Lava;
struct GunUp;
struct EnemyDoor;
struct BigItem;
struct Ooy;

struct JumpScene : Scene
{
	GaemTileMap map;
	JumpMan player;
	SkillTree skillTree;
	DestroyedTiles destroyedTiles;
	DialogDriver dialogDriver;
	RotoText rotoText;
	Entity* boss_bipedal;
	Entity* boss_minotaur;
	Ooy* boss_ooy;
	ZoneManager zoneManager;
	float test_anim_scale;
	int saveSlot;

	EnemyDoor* door_to_close_when_break_skill;

	Lava* raising_lava;
	float raising_lava_target_height;

	std::vector<int> waveShaderScreens;
	bool shaderLavaActive = false;
	bool contextActionButton = false;
	bool playerPaused = false;

	void SaveGame(int saveStation) const;
	void LoadGame();

	JumpScene(int saveSlot = 0);
	~JumpScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;
	bool UpdateCamera(float dt);


	void TriggerPickupItem(BigItem * g, bool fromSave);
};
