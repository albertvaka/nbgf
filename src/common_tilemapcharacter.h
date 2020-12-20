#pragma once

#include "vec.h"
#include "tilemap.h"
#include "screen.h"

// FIXME: Jumping against a slope that goes up in the same direction you are moving behaves weirdly.

inline bool IsGrounded(vec pos, vec size, vec marginPixels = vec(1, 1.5)) {

	float y_bottom = pos.y + (size.y / 2);
	float x_left = pos.x - (size.x / 2);
	float x_right = pos.x + (size.x / 2);

	int ty = TileMap::toTiles(y_bottom + marginPixels.y);
	int tx_left = TileMap::toTiles(x_left + marginPixels.x);
	int tx_right = TileMap::toTiles(x_right - marginPixels.x);

	TileMap* tileMap = TileMap::instance();
	for (int tx = tx_left; tx <= tx_right; tx++) {
		Tile t = tileMap->getTile(tx, ty);
		if (t.isFullSolid()) {
			return true;
		}
		if (t.isOneWay() && (ty * Tile::size) + 1.f > y_bottom) {
			return true;
		}
	}

	// Slopes
	if (TileMap::instance()->isPosOnSlope(pos)) {
		return true;
	}

	return false;
}

inline bool IsMovingTowardsInX(vec pos, vec vel, vec target) {
	return (vel.x < 0 && pos.x > target.x) || (vel.x > 0 && pos.x < target.x);
}

inline bool IsMovingTowardsInY(vec pos, vec vel, vec target) {
	return (vel.y < 0 && pos.y > target.y) || (vel.y > 0 && pos.y < target.y);
}

inline bool IsGoingToRunOffPlatform(vec pos, vec size, vec vel, float dt) {
	vec newPos = pos + vel * dt;
	float toTheSideImMoving = vel.x > 0 ? size.x / 2 : -size.x / 2;
	vec bottomCorner = vec(newPos.x + toTheSideImMoving, newPos.y + size.y / 2);
	//bottomCorner.Debuggerino();
	veci tilePosBottom = TileMap::toTiles(bottomCorner);
	const Tile tileBottom = TileMap::instance()->getTile(tilePosBottom);
	if (tileBottom.isFullSolid()) {
		return false;
	}
	if (tileBottom.isOneWay()) {
		return false;
	}
	return true;
}

inline bool IsGoingToHitAWall(vec pos, vec size, vec vel, float dt) {
	vec newPos = pos + vel * dt;
	float toTheSideImMoving = vel.x > 0 ? size.x / 2 : -size.x / 2;
	vec side = vec(newPos.x + toTheSideImMoving, newPos.y);
	veci tilePosSide = TileMap::toTiles(side);
	const Tile tileSide = TileMap::instance()->getTile(tilePosSide);
	return tileSide.isSolid();
}

inline bool IsGoingToLeaveTheScreen(vec pos, vec size, vec vel, float dt, int screen) {
	vec newPos = pos + vel * dt;
	float toTheSideImMoving = vel.x > 0 ? size.x / 2 : -size.x / 2;
	vec side = vec(newPos.x + toTheSideImMoving, newPos.y);
	return !ScreenManager::instance()->ScreenBounds(screen).Contains(newPos);
}

struct MoveResult {
	Tile leftWallCollision = Tile::NONE;
	Tile rightWallCollision = Tile::NONE;
	Tile ceilingCollision = Tile::NONE;
	Tile groundCollision = Tile::NONE;
	vec pos;
};

//Based on code by: Jordi Santiago
inline MoveResult MoveAgainstTileMap(vec position, vec size, vec vel, float dt) {
	MoveResult ret;

	vec pos = position;
	pos.y += size.y/2; //The code here assumes pos is at the bottom center

	TileMap* map = TileMap::instance();

	vec appliedVel = vel;
	Tile tileAtMyFeet = map->getTile(TileMap::toTiles(pos.x, pos.y - 0.1f));
	if (tileAtMyFeet.isSlope()) {
		// On a slope, we will override the Y displacement with the X displacement, either upwards or downwards.
		// Pythagoras wouldn't approve that we move at the same velocity on a flat X axis than when move on both X and Y.
		// The mathematically accurate value would be /2 but we don't want to slow the player that much either.
		appliedVel.x = sqrt((vel.x * vel.x) / 1.8f);
		if (vel.x < 0) appliedVel.x = -appliedVel.x;
	}
	vec posf = pos + appliedVel * dt;

	//Debug::out << "----------";
	//Debug::out << abs(pos.x - posf.x) / dt;
	//Debug::out << "slope=" << tileAtMyFeet.isSlope();

	vec direction = posf - pos;
	const float E = 1;
	float halfWidth = size.x / 2;

	//Skip horizontal collisions when on a slope
	if (tileAtMyFeet.isSlope()) {
		goto horz_exit;
	}

	if (direction.x < 0) //Moving left
	{

		int xo = map->toTiles(pos.x - halfWidth);
		int xn = map->toTiles(posf.x - halfWidth);
		int yTop = map->toTiles(pos.y - size.y + E);
		int yBottom = map->toTiles(pos.y - E);
		for (int x = xo; x >= xn; x--)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				Tile t = map->getTile(x, y);
				if (t.isFullSolid())
				{
					posf.x = map->Right(x) + halfWidth;
					ret.leftWallCollision = t;
					goto horz_exit;
				}
			}
		}
		//No collision left
	}
	else if (direction.x > 0) //Moving right
	{
		int xo = map->toTiles(pos.x + halfWidth);
		int xn = map->toTiles(posf.x + halfWidth);
		int yTop = map->toTiles(pos.y - size.y + E);
		int yBottom = map->toTiles(pos.y - E);
		for (int x = xo; x <= xn; x++)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				Tile t = map->getTile(x, y);
				if (t.isFullSolid())
				{
					posf.x = map->Left(x) - halfWidth;
					ret.rightWallCollision = t;
					goto horz_exit;
				}
			}
		}
		//No collision right
	}


horz_exit:
	pos.x = posf.x;

	if (direction.y < 0) //Moving up
	{
		int yo = map->toTiles(pos.y - size.y); // top edge
		int yn = map->toTiles(posf.y - size.y);
		int xl = map->toTiles(pos.x - halfWidth + E);
		int xr = map->toTiles(pos.x + halfWidth - E);
		for (int y = yo; y >= yn; y--)
		{
			for (int x = xl; x <= xr; x++)
			{
				Tile t = map->getTile(x, y);
				if (t.isSolid()) //slopes should be collisionable when going up
				{
					posf.y = map->Bottom(y) + size.y;
					ret.ceilingCollision = t;
					goto vert_exit;
				}
			}
		}
		//No collision up
	}
	else if (direction.y > 0) //Moving down
	{
		float max_movement_into_slope = abs(appliedVel.x * dt) + 1.f;

		//Debug::out << "getTile from=" << TileMap::offsetInTile(posf.x, pos.y - max_movement_into_slope) << ": " << map->getTile(TileMap::toTiles(vec(posf.x, pos.y - max_movement_into_slope)));
		//Debug::out << "getTile to=" << TileMap::offsetInTile(posf.x, pos.y + max_movement_into_slope) << ": " << map->getTile(TileMap::toTiles(vec(posf.x, pos.y + max_movement_into_slope)));

		for (int y = floor(pos.y - max_movement_into_slope); y < ceil(pos.y + max_movement_into_slope); y++) {
			float E = 0.0001f;
			if (map->isPosOnSlope(posf.x, y - E)) { // hack: we want to get to the edge of a tile before stepping onto the next one, hence the epsilon deduced from the integer value.
				posf.y = y;
				ret.groundCollision = map->getTile(posf.x, y - E);
				goto vert_exit;
			}
		}

		int yo = map->toTiles(pos.y); // bottom edge
		int yn = map->toTiles(posf.y);
		int xl = map->toTiles(pos.x - halfWidth + E);
		int xr = map->toTiles(pos.x + halfWidth - E);
		for (int y = yo; y <= yn; y++)
		{
			for (int x = xl; x <= xr; x++)
			{
				Tile t = map->getTile(x, y);
				if ((t.isFullSolid()) || (t.isOneWay() && pos.y - 1.f < (y * Tile::size)))
				{
					posf.y = map->Top(y);
					ret.groundCollision = t;
					goto vert_exit;
				}
			}
		}
		//No collision down
	}
vert_exit:
	posf.y -= size.y / 2; // The code here assumes the pos is at to bottom center
	ret.pos = posf;
	return ret;
}

/*
inline vec posOnRightSlope(vec pos) {
	vec rounded = (TileMap::toTiles(pos) * Tile::size);
	vec diff = pos - rounded;
	float targetY = (Tile::size - diff.x);
	if (targetY < diff.y) {
		pos.y = rounded.y + targetY;
	}
	return pos;
}

inline vec posOnLeftSlope(vec pos) {
	vec rounded = (TileMap::toTiles(pos) * Tile::size);
	vec diff = pos - rounded;
	float targetY = diff.x;
	if (targetY < diff.y) {
		pos.y = rounded.y + targetY;
	}
	return pos;
}
*/