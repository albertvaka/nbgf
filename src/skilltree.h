#pragma once

#include "singleinstance.h"
#include <vector>
#include "tile.h"

enum class Skill {
	NO = 0,
	GUN,
	WALLJUMP,
	BREAK,
	DAMAGE_UP,
	DASH,
	DIVE,
	ATTACK,
};

struct SaveState;

struct SkillTree : SingleInstance<SkillTree>
{
	SkillTree();

	Tile::BreakResistance GetBreakPower() const {
		if (IsEnabled(Skill::DAMAGE_UP)) return Tile::BreakResistance::HARD_OR_LOWER;
		if (IsEnabled(Skill::BREAK)) return Tile::BreakResistance::SOFT;
		return Tile::BreakResistance::NONE;
	}
	
	int GetDamage() const {
		if (IsEnabled(Skill::DAMAGE_UP)) return 2;
		return 1;
	}

	bool IsEnabled(Skill s) const {
		return enabled[int(s)];
	}

	void Enable(Skill s) {
		enabled[int(s)] = true;
	}

	void Reset();

	int gunpoints = 0;
	bool open = false;

	void SaveGame(SaveState& state) const;
	void LoadGame(const SaveState& state);

private:
	std::vector<bool> enabled;
};

