#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "net.h"

constexpr float stage_duration = 3.0f;

enum INTRO_STAGES {
    OVERSEER_CLOSE_EYES,
	SHOW_ARROWS,
	GAME
};

struct SceneMain : Scene {
	bool is_server;

	INTRO_STAGES curr_stage;
	float time_until_next_stage = stage_duration;
	Text close_eyes_text;

	SceneMain(bool is_server);

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void SpawnCity();
	void BPS(veci min, veci max, int level);
};
