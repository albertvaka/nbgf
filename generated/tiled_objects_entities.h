#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Entities {

	// Positions
	static const std::unordered_map<int, vec> bat;
	static const vec single_bat;
	static const std::unordered_map<int, vec> debug_draw;
	static const vec single_debug_draw;
	static const std::unordered_map<int, vec> enemy_door;
	static const std::unordered_map<int, vec> fireslime;
	static const std::unordered_map<int, vec> frame_by_frame;
	static const vec single_frame_by_frame;
	static const std::unordered_map<int, vec> holi;
	static const vec single_holi;
	static const std::unordered_map<int, vec> holi_2;
	static const vec single_holi_2;
	static const std::unordered_map<int, vec> lava_initial_height;
	static const vec single_lava_initial_height;
	static const std::unordered_map<int, vec> miniooy;
	static const std::unordered_map<int, vec> move_cam;
	static const vec single_move_cam;
	static const std::unordered_map<int, vec> rocket_launcher;
	static const std::unordered_map<int, vec> save;
	static const vec single_save;
	static const std::unordered_map<int, vec> spawn;
	static const vec single_spawn;
	// Bounds
	static const std::array<BoxBounds, 1> imgui;
	static const BoxBounds single_imgui;

};

}