#pragma once

#include "scene.h"
#include "tilemap.h"
#include "jumpman.h"
#include "lava.h"
#include "partsys.h"
#include "destroyedtiles.h"
#include "skilltree.h"
#include "screen.h"

struct HellCrossScene : Scene {

	int randomSeed;
	TileMap map;
	JumpMan player;
	Lava lava;
	PartSys bulletPartSys;
	SkillTree skillTree;
	DestroyedTiles destroyedTiles;
	ScreenManager sceneManager;
	GPU_Image* renderToTextureTarget;
	float introTime;

	HellCrossScene();

	void RandomizeMap();

	void EnterScene();
	void ExitScene();
	void Update(float dt);
	void Draw();
};
