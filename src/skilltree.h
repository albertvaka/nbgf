#pragma once

#include "singleinstance.h"
#include "savestate.h"
#include "magic_enum.h"
#include <vector>
#include "text.h"

enum class Skill {
	NO = 0,
	GUN,
	WALLJUMP,
	BREAK,
	BOUNCY,
	RANGE_1,
	RAPID_FIRE,
	DMG_1,
	RANGE_2,
};

struct SkillTree : SingleInstance<SkillTree>
{
	SkillTree();
	void Update(float dt);
	void DrawMenu();
	void DrawOverlay();

	bool IsEnabled(Skill s) const {
		return enabled[int(s)];
	}

	void Enable(Skill s) {
		enabled[int(s)] = true;
	}

	int gunpoints = 0;
	bool open = false;

	void SaveGame(SaveState& state) const {
		for (Skill skill : magic_enum::enum_values<Skill>()) {
			state.StreamPut("skills_" + std::string(magic_enum::enum_name(skill))) << enabled[int(skill)];
		}
	};

	void LoadGame(const SaveState& state) {
		for (Skill skill : magic_enum::enum_values<Skill>()) {
			bool b;
			if (state.StreamGet("skills_" + std::string(magic_enum::enum_name(skill))) >> b) {
				enabled[int(skill)] = b;
			}
		}
	}

private:

	std::vector<bool> enabled;
	veci current;
	int prev_left = -1;
	int prev_right = -1;

	int requirements_not_met_skill = -1;
	float requirements_not_met_timer = 0.f;

	float not_enough_points_timer = 0.f;

	Text textPoints;
	Text textDescription;
	Text textPressStart;
};

