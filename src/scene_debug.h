#pragma once

#include "scene.h"
#include "gaemtilemap.h"
#include "jumpman.h"
#include "lava.h"
#include "partsys.h"
#include "destroyedtiles.h"
#include "skilltree.h"
#include "screen.h"

struct DebugScene : Scene {

	int randomSeed;
	
	GaemTileMap map;
	JumpMan player;
	SkillTree skillTree;
	DestroyedTiles destroyedTiles;
	ScreenManager sceneManager;

	DebugScene();

	void RandomizeMap();
	void EnterScene();
	void ExitScene();
	void Update(float dt);
	void UpdateCamera();
	void Draw();
};
