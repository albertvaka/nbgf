#pragma once

#include "singleinstance.h"
#include <vector>
#include "text.h"

struct SkillTree : SingleInstance<SkillTree>
{
	enum Skill {
		BREAK = 0,
		BOUNCY = 1,
		RANGE_1 = 2,
		RAPID_FIRE = 3,
		DMG_1 = 4,
		RANGE_2 = 5,
	};

	SkillTree();
	void Update(float dt);
	void DrawMenu();
	void DrawOverlay();

	struct TreePos {
		int i, j;
	};
	bool open = false;
	int gunpoints = 0;
	TreePos current;
	int prev_left = -1;
	int prev_right = -1;

	int requirements_not_met_skill = -1;
	float requirements_not_met_timer = 0.f;

	float not_enough_points_timer = 0.f;

	std::vector<bool> enabled;

	Text textPoints;
	Text textDescription;
	Text textPressStart;

};

