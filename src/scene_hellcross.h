#pragma once

#include "scene.h"
#include "gaemtilemap.h"
#include "player.h"
#include "lava.h"
#include "destroyedtiles.h"
#include "text.h"
#include "skilltree.h"
#include "screen.h"

struct Text;

struct HellCrossScene : Scene
{
	int randomSeed;
	GaemTileMap map;
	Player player;
	Lava lava;
	SkillTree skillTree;
	DestroyedTiles destroyedTiles;
	Text scoreText;
	Text bestScoreText;
	int score;
	int bestScore;
	SaveState save;

	HellCrossScene();
	~HellCrossScene();

	void RandomizeMap();

	void EnterScene();
	void ExitScene();
	void Update(float dt);
	void UpdateCamera();
	void Draw();
	void UpdateScores();
};
