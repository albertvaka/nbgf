#pragma once

#include "vec.h"
#include "bounds.h"
#include "mates.h"
#include "SDL_gpu.h"

namespace Tiled {

struct Tile {
	
	static constexpr const int Size = 16;
	static constexpr const vec Sizes = vec(16,16);

	static const GPU_Rect TileToTextureRect[];
	constexpr const GPU_Rect& textureRect() const { return TileToTextureRect[int(value)]; }

	// Coordinate conversion functions

	static veci ToTiles(vec pos) { return ToTiles(pos.x, pos.y); }
	static veci ToTiles(float x, float y) { return veci(ToTiles(x), ToTiles(y)); }
	static int ToTiles(float x) { return Mates::fastfloor(x / Tile::Size); } // floor could be just a cast to int if we know we will never get < 0

	static vec FromTiles(int x, int y) { return vec(x * Tile::Size, y * Tile::Size); }
	static vec FromTiles(veci pos) { return FromTiles(pos.x, pos.y);  }

	static vec AlignToTiles(vec v) { return ToTiles(v) * Tile::Size; }
	static vec AlignToTiles(float x, float y) { return ToTiles(x, y) * Tile::Size; }
	static float AlignToTiles(float x) { return ToTiles(x) * Tile::Size; }

	static vec OffsetInTile(float x, float y) { return vec(x, y) - AlignToTiles(x, y); }

	static float Bottom(int y) { return float(y + 1) * Tile::Size; }
	static float Top(int y) { return float(y) * Tile::Size; }
	static float Left(int x) { return float(x) * Tile::Size; }
	static float Right(int x) { return float(x + 1) * Tile::Size; }
	
	static BoxBounds Bounds(int x, int y) { return BoxBounds(x * Tile::Size, y * Tile::Size, Tile::Size, Tile::Size); }
	static BoxBounds Bounds(veci pos) { return Bounds(pos.x, pos.y); }

	enum Value : uint16_t
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
		BG_128,
		BG_129,
		BG_130,
		BG_131,
		BG_132,
		BG_133,
		BG_134,
		BG_135,
		BG_136,
		BG_137,
		BG_138,
		BG_139,
		BG_140,
		BG_141,
		BG_142,
		BG_143,
		BG_144,
		BG_145,
		BG_146,
		BG_147,
		BG_148,
		BG_149,
		BG_150,
		BG_151,
		BG_152,
		BG_153,
		BG_154,
		BG_155,
		BG_156,
		BG_157,
		BG_158,
		BG_159,
		BG_160,
		BG_161,
		BG_162,
		BG_163,
		BG_164,
		BG_165,
		BG_166,
		BG_167,
		BG_168,
		BG_169,
		BG_170,
		BG_171,
		BG_172,
		BG_173,
		BG_174,
		BG_175,
		BG_176,
		BG_177,
		BG_178,
		BG_179,
		BG_180,
		BG_181,
		BG_182,
		BG_183,
		BG_184,
		BG_185,
		BG_186,
		BG_187,
		BG_188,
		BG_189,
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
		RSLOPE_1,
		RSLOPE_2,
		LSLOPE_1,
		LSLOPE_2,
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
		SOLID_38,
		SOLID_39,
		SOLID_40,
		SOLID_41,
		SOLID_42,
		SOLID_43,
		SOLID_44,
		SOLID_45,
		SOLID_46,
		SOLID_47,
		SOLID_48,
		SOLID_49,
		SOLID_50,
		SOLID_51,
		SOLID_52,
		BREAKABLE_1,
		BREAKABLE_2,
		BREAKABLE_3,
		BREAKABLE_4,
		BREAKABLE_HARD_1,
		BREAKABLE_HARD_2,
		BREAKABLE_GND_1,
		SOLID_TRANSPARENT,
	};

	// Aliases
	static constexpr Value BG_CAVE_CEILING_BEHIND_BREAKABLE = BG_17;
	static constexpr Value BG_CAVE_FLOOR_BEHIND_BREAKABLE = BG_60;
	static constexpr Value BG_DOOR_OPENING = BG_81;
	static constexpr Value ONEWAY_BEHIND_BREAKABLE = ONEWAY_9;
	static constexpr Value ONEWAY_SIMPLE = ONEWAY_9;
	static constexpr Value SOLID_SIMPLE = SOLID_12;
	static constexpr Value SOLID_DOOR = SOLID_33;
	static constexpr Value SOLID_DOOR_BOTTOM = SOLID_40;
	static constexpr Value SOLID_OUT_OF_BOUNDS = SOLID_46;
	static constexpr Value BREAKABLE_COVERING_ONEWAY = BREAKABLE_1;
	static constexpr Value BREAKABLE_COVERING_CAVE_FLOOR = BREAKABLE_2;
	static constexpr Value BREAKABLE_COVERING_CAVE_CEILING = BREAKABLE_3;
	static constexpr Value BREAKABLE_SIMPLE = BREAKABLE_4;
	static constexpr Value BREAKABLE_HARD_EXPLOSIVE_BLOCK = BREAKABLE_HARD_1;

	constexpr operator Value() const { return value; }  // Allow switch and comparisons.
	explicit constexpr operator bool() = delete;        // Prevent if(tile)

	Tile() = default;
	constexpr Tile(Value t) : value(t) { }

protected:
	Value value;
};

}