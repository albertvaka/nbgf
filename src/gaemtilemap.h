#pragma once

#include "singleinstance.h"
#include "tilemap.h"
#include "tile.h"

#ifdef _DEBUG
#include "input.h"
#endif

struct GaemTileMap : TileMap<Tile>, SingleInstance<GaemTileMap>
{
	GaemTileMap(int width, int height, GPU_Image* texture) : TileMap(width, height, texture) {
		outOfBoundsTile = Tile::SOLID_OUT_OF_BOUNDS;
	}

	bool IsPosBelowSlope(veci tilePos) const {
		veci aboveTilePos = veci(tilePos.x, tilePos.y - 1);
		Tile aboveTile = GetTile(aboveTilePos);
		if (aboveTile.isRightSlope()) {
			veci rightTilePos = veci(tilePos.x - 1, tilePos.y);
			Tile rightTile = GetTile(rightTilePos);
			return rightTile.isRightSlope();
		}
		if (aboveTile.isLeftSlope()) {
			veci leftTilePos = veci(tilePos.x + 1, tilePos.y);
			Tile leftTile = GetTile(leftTilePos);
			return leftTile.isLeftSlope();
		}
		return false;
	}

	bool IsPosOnSlope(vec v) const { return IsPosOnSlope(v.x, v.y); }
	bool IsPosOnSlope(float x, float y) const {
		veci tilePos = Tile::ToTiles(x, y);
		if (IsPosBelowSlope(tilePos)) {
			return true;
		}
		return GetTile(tilePos).IsInSolidPartOfSlope(x, y);
	}

	bool CollidesWithSlope(vec pos, float velY, float dt) {
		float finalY = pos.y + velY * dt;
		while (pos.y <= finalY) {
			if (IsPosOnSlope(pos.x, pos.y)) {
				return true;
			}
			pos.y += 1.f;
		}
		return false;
	}

#ifdef _DEBUG
	int debugEditCurrentTile = Tile::NONE;

	void DebugEdit()
	{
		if (Mouse::GetScrollWheelMovement() < 0.f) {
			debugEditCurrentTile -= 1;
			if (debugEditCurrentTile < 1) debugEditCurrentTile = magic_enum::enum_count<Tile::Value>() - 1;
		}
		else if (Mouse::GetScrollWheelMovement() > 0.f) {
			debugEditCurrentTile += 1;
			if (debugEditCurrentTile >= int(magic_enum::enum_count<Tile::Value>())) debugEditCurrentTile = 1;
		}
		if (Input::IsPressed(0, GameKeys::LEFT)) debugEditCurrentTile = Tile::LSLOPE_1;
		if (Input::IsPressed(0, GameKeys::RIGHT)) debugEditCurrentTile = Tile::RSLOPE_1;
		if (Input::IsPressed(0, GameKeys::JUMP)) debugEditCurrentTile = Tile::ONEWAY_1;
		if (Input::IsPressed(0, GameKeys::CROUCH)) debugEditCurrentTile = Tile::SOLID_1;
		bool left = Mouse::IsPressed(Mouse::Button::Left);
		bool right = Mouse::IsPressed(Mouse::Button::Right);
		if (left || right) {
			Tile what_to_set = left ? Tile::Value(debugEditCurrentTile) : Tile::NONE;
			vec pos = Mouse::GetPositionInWorld();
			veci tile = veci(pos.x/Tile::Size, pos.y/Tile::Size);
			SetTile(tile.x, tile.y, what_to_set);
		}
	}

	void DebugEditDraw() const
	{
		vec pos = Camera::Bounds().TopLeft() + vec(0, 16);
		BoxBounds(pos, vec(Tile::Size, Tile::Size)).DebugDraw(0, 0, 0);
		Window::Draw(tileset, pos)
			.withRect(Tile::TileToTextureRect[debugEditCurrentTile]);
	}
#endif

};
