#pragma once

#include "tiled_tile.h"

struct Tile : Tiled::Tile
{
	bool isEmpty() const {
		return value == NONE;
	}

	bool isInvisible() const {
		return false;
	}

	bool isSolid() const {
		return value >= SOLID_1;
	}

	// Boilerplate

	Tile() = default;
	constexpr Tile(Value t) : Tiled::Tile(t) { }

};
