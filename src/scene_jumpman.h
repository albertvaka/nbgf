#pragma once

#include "mates.h"
#include "tilemap.h"
#include "jumpman.h"
#include "scene_manager.h"
#include "npc.h"
#include "input.h"
#include "rototext.h"

struct JumpScene : Scene {

	TileMap map;
	Npc npc;
	RotoText rototext;

	bool lost = false;
	int moneys = 150;
	int cantbuyTimer = 0;

	Text totalMoneyText;
	Text moneyText;
	float moneyTextTimer = 0;
	int current_music = 0;
	float timerDoggo = 0;
	int first_player = 0;
	JumpMan* players[Input::kMaxPlayers];
	GameKeys contextActionButton[Input::kMaxPlayers];

	JumpScene(int first_player = 0);
	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

};
