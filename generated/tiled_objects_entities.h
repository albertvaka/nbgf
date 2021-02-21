#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Entities {

	// Objects
	static const std::unordered_map<int, vec> angrybat;
	static const std::unordered_map<int, vec> bat;
	static const std::unordered_map<int, vec> batawake;
	static const vec boss_bipedal;
	static const vec debug_teleport;
	static const std::unordered_map<int, vec> enemy_door;
	static const std::unordered_map<int, vec> fireslime;
	static const std::unordered_map<int, vec> flyingalien;
	static const std::unordered_map<int, vec> goomba;
	static const std::unordered_map<int, vec> goombacharger;
	static const std::unordered_map<int, vec> healthup;
	static const std::unordered_map<int, vec> initial_batawake;
	static const vec lava_initial_height;
	static const std::unordered_map<int, vec> mantis;
	static const std::unordered_map<int, vec> save;
	static const vec skill_breakblocks;
	static const vec skill_gun;
	static const vec skill_walljump;
	static const vec spawn;

};

}