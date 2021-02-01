#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "rototext.h"

enum INTRO_STAGES {
    OVERSEER_CLOSE_EYES,
	SHOW_ARROWS,
	GAME
};

struct SceneMain : Scene {
	INTRO_STAGES curr_stage;
	float time_until_next_stage;

	Text textTime;
	bool gameover;
	
	float corner_anim_timer;
	RotoText rotoText;

	int num_goals;

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void SpawnCity();
	void BPS(veci min, veci max, int level);
};
