#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"

struct MainScene : Scene {

	Player player;
	Text timerText;
	bool player_invincible = false;

	MainScene();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

private:
	void PlayerDie();
};
