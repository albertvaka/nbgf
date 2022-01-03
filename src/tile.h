#pragma once

#include "tiled_tile.h"

struct Tile : Tiled::Tile
{
	bool isEmpty() const {
		return value < ONEWAY_1;
	}

	bool isOneWay() const {
		return value >= ONEWAY_1 && value < RSLOPE_1;
	}

	bool isSlope() const {
		return value >= RSLOPE_1 && value < SOLID_1;
	}

	bool isLeftSlope() const {
		return value >= LSLOPE_1 && value < SOLID_1;
	}

	bool isRightSlope() const {
		return value >= RSLOPE_1 && value < LSLOPE_1;
	}

	bool isInvisible() const {
		return value == NONE || value == SOLID_TRANSPARENT;
	}

	bool isSolid() const {
		return value >= RSLOPE_1;
	}

	bool isFullSolid() const { // Excludes slopes
		return value >= SOLID_1;
	}

	bool IsInSolidPartOfSlope(float x, float y) {
		if (isRightSlope()) {
			vec offset = Tile::OffsetInTile(x, y);
			return offset.y >= (Tile::Size - offset.x);
		}
		if (isLeftSlope()) {
			vec offset = Tile::OffsetInTile(x, y);
			return offset.y >= offset.x;
		}
		return false;
	}

	bool isSafeGround() const {
		return (isSolid() && !isBreakable(BreakResistance::ANY) && !isBreakableGround()) || isOneWay(); // maybe oneway shouldn't count
	}

	Tile GetTileBehind() const { // What's "behind" of this tile when it breaks
		if (value == Tile::BREAKABLE_COVERING_ONEWAY || isBreakableGround()) {
			return Tile::ONEWAY_BEHIND_BREAKABLE;
		} else if (value == Tile::BREAKABLE_COVERING_CAVE_CEILING) {
			return Tile::BG_CAVE_CEILING_BEHIND_BREAKABLE;
		} else if (value == Tile::BREAKABLE_COVERING_CAVE_FLOOR) {
			return Tile::BG_CAVE_FLOOR_BEHIND_BREAKABLE;
		} else {
			return Tile::NONE;
		}
	}

	bool IsChainBreakable() const { // Breaking it breaks the neighbouring tiles of the same type
		return isBreakableGround() || (isBreakable(BreakResistance::HARD_OR_LOWER) && !isBreakable(BreakResistance::SOFT)) || value == BREAKABLE_COVERING_ONEWAY;
	}

	enum class BreakResistance {
		NONE = 0,
		SOFT,
		HARD,
		HARD_OR_LOWER,
		ANY = HARD_OR_LOWER,
	};

	bool isHardBreakable() const {
		return value >= BREAKABLE_HARD_1 && value < BREAKABLE_GND_1; // Exclude soft and ground breakable tiles
	}

	bool isBreakable(BreakResistance breakPower) const {
		if (breakPower == BreakResistance::NONE) return false;
		if (breakPower == BreakResistance::SOFT) return value >= BREAKABLE_1 && value < BREAKABLE_HARD_1; // Exclude hard breakable tiles
		if (breakPower == BreakResistance::HARD) return value >= BREAKABLE_HARD_1 && value < BREAKABLE_GND_1; // Exclude hard breakable tiles
		return value >= BREAKABLE_1 && value < BREAKABLE_GND_1; // Soft and hard breakable tiles
	}

	bool isBreakableGround() const {
		return value >= BREAKABLE_GND_1 && value < SOLID_TRANSPARENT;
	}

	// Boilerplate

	Tile() = default;
	constexpr Tile(Value t) : Tiled::Tile(t) { }

};
