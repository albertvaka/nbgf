#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "text.h"
#include "rototext.h"
#include "partsys.h"
#include "camera.h"

struct Goals : CircleEntity {
	Goals();

	enum class State {
		NOT_ACTIVE,
		ACTIVE,
		GAME_OVER,
	};

	State state;

	void Reset();
	void Update(float dt);
	void Draw() const;
	void DrawGui() const;

	void GotGoal();
	void GameOver();

	PartSys activeGoal;
	PartSys inactiveGoal;

	float countdown;
	Text countdownText;
	RotoText gameOverRotoText;
	
	float restartTimer;
	Text restartText;
};
