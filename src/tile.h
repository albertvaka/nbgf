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

	bool isFullSolid() const { //Excludes slopes
		return value >= SOLID_1;
	}

	bool isBreakable() const {
		return value >= BREAKABLE_1 && value < SOLID_TRANSPARENT;
	}

	// Boilerplate

	Tile() = default;
	constexpr Tile(Value t) : Tiled::Tile(t) { }

};