#include "skilltree.h"

#include "magic_enum.h"
#include "savestate.h"

#include <algorithm>

SkillTree::SkillTree() {
	enabled.resize(magic_enum::enum_count<Skill>(), false);
}

void SkillTree::Reset() {
	std::fill(enabled.begin(), enabled.end(), false);
}

void SkillTree::SaveGame(SaveState& state) const {
	for (Skill skill : magic_enum::enum_values<Skill>()) {
		state.StreamPut("skills_" + std::string(magic_enum::enum_name(skill))) << enabled[int(skill)];
	}
};

void SkillTree::LoadGame(const SaveState& state) {
	for (Skill skill : magic_enum::enum_values<Skill>()) {
		bool b = false;
		state.StreamGet("skills_" + std::string(magic_enum::enum_name(skill))) >> b;
		enabled[int(skill)] = b;
	}
}
