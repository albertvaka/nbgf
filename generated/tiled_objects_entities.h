#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Entities {

	// Objects
	static const std::unordered_map<int, vec> angrybat;
	static const std::unordered_map<int, vec> angrybatawake;
	static const vec single_angrybatawake;
	static const std::unordered_map<int, vec> bat;
	static const std::unordered_map<int, vec> batawake;
	static const std::unordered_map<int, vec> boss_bipedal;
	static const vec single_boss_bipedal;
	static const std::unordered_map<int, vec> debug_teleport;
	static const vec single_debug_teleport;
	static const std::unordered_map<int, vec> enemy_door;
	static const std::unordered_map<int, vec> explosive;
	static const vec single_explosive;
	static const std::unordered_map<int, vec> fireslime;
	static const std::unordered_map<int, vec> flyingalien;
	static const std::unordered_map<int, vec> goomba;
	static const vec single_goomba;
	static const std::unordered_map<int, vec> goombacharger;
	static const vec single_goombacharger;
	static const std::unordered_map<int, vec> healthup;
	static const std::unordered_map<int, vec> initial_batawake;
	static const std::unordered_map<int, vec> lava_initial_height;
	static const vec single_lava_initial_height;
	static const std::unordered_map<int, vec> mantis;
	static const std::unordered_map<int, vec> minotaur;
	static const vec single_minotaur;
	static const std::unordered_map<int, vec> rocket_launcher;
	static const std::unordered_map<int, vec> save;
	static const std::unordered_map<int, vec> skill_attack;
	static const vec single_skill_attack;
	static const std::unordered_map<int, vec> skill_breakblocks;
	static const vec single_skill_breakblocks;
	static const std::unordered_map<int, vec> skill_dive;
	static const vec single_skill_dive;
	static const std::unordered_map<int, vec> skill_gun;
	static const vec single_skill_gun;
	static const std::unordered_map<int, vec> skill_walljump;
	static const vec single_skill_walljump;
	static const std::unordered_map<int, vec> spawn;
	static const vec single_spawn;
	static const std::unordered_map<int, vec> temp_explosive;
	static const vec single_temp_explosive;

};

}