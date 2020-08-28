#pragma once

#include <array>

#include "vec.h"
#include "bounds.h"
#include "SDL_gpu.h"

struct TiledTiles
{
	static const GPU_Rect tileToTextureRect[];

	enum Value : uint8_t
	{
		NONE = 0,
		BG_1,
		BG_2,
		BG_3,
		BG_4,
		BG_5,
		BG_6,
		BG_7,
		BG_8,
		BG_9,
		BG_10,
		BG_11,
		BG_12,
		BG_13,
		BG_14,
		BG_15,
		BG_16,
		BG_17,
		BG_18,
		BG_19,
		BG_20,
		BG_21,
		BG_22,
		BG_23,
		BG_24,
		BG_25,
		BG_26,
		BG_27,
		BG_28,
		BG_29,
		BG_30,
		BG_31,
		BG_32,
		BG_33,
		BG_34,
		BG_35,
		BG_36,
		BG_37,
		BG_38,
		BG_39,
		BG_40,
		BG_41,
		BG_42,
		BG_43,
		BG_44,
		BG_45,
		BG_46,
		BG_47,
		BG_48,
		BG_49,
		BG_50,
		BG_51,
		BG_52,
		BG_53,
		BG_54,
		BG_55,
		BG_56,
		BG_57,
		BG_58,
		BG_59,
		BG_60,
		BG_61,
		BG_62,
		BG_63,
		BG_64,
		BG_65,
		BG_66,
		BG_67,
		BG_68,
		BG_69,
		BG_70,
		BG_71,
		BG_72,
		BG_73,
		BG_74,
		BG_75,
		BG_76,
		BG_77,
		BG_78,
		BG_79,
		BG_80,
		BG_81,
		BG_82,
		BG_83,
		BG_84,
		BG_85,
		BG_86,
		BG_87,
		BG_88,
		BG_89,
		BG_90,
		BG_91,
		BG_92,
		BG_93,
		BG_94,
		BG_95,
		BG_96,
		BG_97,
		BG_98,
		BG_99,
		BG_100,
		BG_101,
		BG_102,
		BG_103,
		BG_104,
		BG_105,
		BG_106,
		BG_107,
		BG_108,
		BG_109,
		BG_110,
		BG_111,
		BG_112,
		BG_113,
		BG_114,
		BG_115,
		BG_116,
		BG_117,
		BG_118,
		BG_119,
		BG_120,
		BG_121,
		BG_122,
		BG_123,
		BG_124,
		BG_125,
		BG_126,
		BG_127,
		ONEWAY_1,
		ONEWAY_2,
		ONEWAY_3,
		ONEWAY_4,
		ONEWAY_5,
		ONEWAY_6,
		ONEWAY_7,
		ONEWAY_8,
		ONEWAY_9,
		ONEWAY_10,
		ONEWAY_11,
		ONEWAY_12,
		ONEWAY_13,
		ONEWAY_14,
		ONEWAY_15,
		ONEWAY_16,
		ONEWAY_17,
		ONEWAY_18,
		RSLOPE_1,
		RSLOPE_2,
		RSLOPE_3,
		RSLOPE_4,
		LSLOPE_1,
		LSLOPE_2,
		LSLOPE_3,
		LSLOPE_4,
		SOLID_1,
		SOLID_2,
		SOLID_3,
		SOLID_4,
		SOLID_5,
		SOLID_6,
		SOLID_7,
		SOLID_8,
		SOLID_9,
		SOLID_10,
		SOLID_11,
		SOLID_12,
		SOLID_13,
		SOLID_14,
		SOLID_15,
		SOLID_16,
		SOLID_17,
		SOLID_18,
		SOLID_19,
		SOLID_20,
		SOLID_21,
		SOLID_22,
		SOLID_23,
		SOLID_24,
		SOLID_25,
		SOLID_26,
		SOLID_27,
		SOLID_28,
		SOLID_29,
		SOLID_30,
		SOLID_31,
		SOLID_32,
		SOLID_33,
		SOLID_34,
		SOLID_35,
		SOLID_36,
		SOLID_37,
		BREAKABLE_1,
		BREAKABLE_2,
		BREAKABLE_3,
		BREAKABLE_4,
		SOLID_TRANSPARENT,
	};

	static constexpr Value ONEWAY_BEGIN = ONEWAY_1;
	static constexpr Value RSLOPE_BEGIN = RSLOPE_1;
	static constexpr Value LSLOPE_BEGIN = LSLOPE_1;
	static constexpr Value SOLID_BEGIN = SOLID_1;
	static constexpr Value BREAKABLE_BEGIN = BREAKABLE_1;

	// Aliases
	static constexpr Value BG_PLAIN_COLOR = BG_33;
	static constexpr Value BG_DOOR_OPENING = BG_48;
	static constexpr Value ONEWAY_BEHIND_BREAKABLE = ONEWAY_9;
	static constexpr Value ONEWAY_SIMPLE = ONEWAY_9;
	static constexpr Value SOLID_OUT_OF_BOUNDS = SOLID_3;
	static constexpr Value SOLID_SIMPLE = SOLID_6;
	static constexpr Value SOLID_PLAIN_COLOR = SOLID_19;
	static constexpr Value SOLID_DOOR = SOLID_20;
	static constexpr Value SOLID_DOOR_BOTTOM = SOLID_24;
	static constexpr Value BREAKABLE_SIMPLE = BREAKABLE_3;
	static constexpr Value BREAKABLE_COVERING_ONEWAY = BREAKABLE_4;

};

struct TiledMap
{
	static const uint8_t map[];
	static const vec map_size;
	static const std::array<Bounds, 11> screens;
};

struct TiledEntities
{
	static const std::array<vec, 10> batawake;
	static const std::array<vec, 8> bat;
	static const std::array<vec, 2> healthup;
	static const vec spawn;
	static const vec gun;
	static const std::array<vec, 3> enemy_door;
	static const std::array<vec, 2> angrybat;
	static const vec walljump;
	static const std::array<vec, 2> gunup;
	static const vec gunup_tancaporta;
	static const std::array<vec, 2> save;
	static const vec lava_initial_height;
	
};

struct TiledAreas
{
	static const std::array<Bounds, 2> lava;
	static const std::array<Bounds, 1> parallax_forest;
	static const std::array<Bounds, 2> bat_bounds;
	static const std::array<Bounds, 1> fog;
	
};