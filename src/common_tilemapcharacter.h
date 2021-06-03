#pragma once

#include "vec.h"
#include "gaemtilemap.h"
#include "screen.h"
#include "bounds.h"

inline bool IsMovingTowardsInX(vec pos, vec vel, vec target) {
	return (vel.x < 0 && pos.x > target.x) || (vel.x > 0 && pos.x < target.x);
}

inline bool IsMovingTowardsInY(vec pos, vec vel, vec target) {
	return (vel.y < 0 && pos.y > target.y) || (vel.y > 0 && pos.y < target.y);
}

inline vec AlignWithGround(vec pos, vec size) {
	const int marginPixels = 2; // If the entity is sunken into the ground by this much, we will push it up instead of down to the next tile
	pos.y = Tile::Bottom(Tile::ToTiles(pos.y + size.y / 2 - marginPixels)) - size.y / 2;
	return pos;
}

inline vec AlignWithCeiling(vec pos, vec size) {
	const int marginPixels = 2; // If the entity is sunken into the ceiling by this much, we will push it down instead of up to the next tile
	pos.y = Tile::Top(Tile::ToTiles(pos.y + size.y / 2 + marginPixels)) - size.y / 2;
	return pos;
}

inline bool IsGoingToRunOffPlatform(vec pos, vec size, vec vel, float dt) {
	vec newPos = pos + vel * dt;
	float toTheSideImMoving = vel.x > 0 ? size.x / 2 : -size.x / 2;
	vec bottomCorner = vec(newPos.x + toTheSideImMoving, newPos.y + size.y / 2);
	//bottomCorner.DebugDraw();
	veci tilePosBottom = Tile::ToTiles(bottomCorner);
	const Tile tileBottom = GaemTileMap::instance()->GetTile(tilePosBottom);
	if (tileBottom.isFullSolid()) {
		return false;
	}
	if (tileBottom.isOneWay()) {
		return false;
	}
	return true;
}

inline bool IsGoingToHitAWall(vec pos, vec size, vec vel, float dt) {
	GaemTileMap* map = GaemTileMap::instance();
	vec newPos = pos + vel * dt;
	float toTheSideImMoving = vel.x > 0 ? size.x / 2 : -size.x / 2;
	if (size.y < Tile::Size) {
		vec side = vec(newPos.x + toTheSideImMoving, newPos.y);
		veci tilePosSide = Tile::ToTiles(side);
		return map->GetTile(tilePosSide).isSolid();
	} else {
		const float E = 1;
		int x = Tile::ToTiles(newPos.x + toTheSideImMoving);
		int yTop = Tile::ToTiles(newPos.y - size.y/2 + E);
		int yBottom = Tile::ToTiles(pos.y + size.y/2 - E);
		for (int y = yTop; y <= yBottom; y++)
		{
			if (map->GetTile(x, y).isSolid()) {
				return true;
			}
		}
		return false;
	}
}

inline bool IsGoingToLeaveBounds(vec pos, vec size, vec vel, float dt, BoxBounds bounds) {
	vec newPos = pos + vel * dt;
	float toTheSideImMoving = vel.x > 0 ? size.x / 2 : -size.x / 2;
	vec sidePos = vec(newPos.x + toTheSideImMoving, newPos.y);
	return !bounds.Contains(sidePos);
}

inline bool IsGoingToLeaveTheScreen(vec pos, vec size, vec vel, float dt, int screen) {
	if (screen < 0) return false;
	return IsGoingToLeaveBounds(pos, size, vel, dt, ScreenManager::instance()->ScreenBounds(screen));
}

struct MoveResult {
	Tile leftWallCollision = Tile::NONE;
	Tile rightWallCollision = Tile::NONE;
	Tile ceilingCollision = Tile::NONE;
	Tile groundCollision = Tile::NONE;
	veci groundCollisionPos = veci(0,0);
	vec pos = vec(0.f,0.f);
};

//Based on code by: Jordi Santiago
inline MoveResult MoveAgainstTileMap(vec position, vec size, vec vel, float dt) {
	MoveResult ret;

	vec pos = position;
	pos.y += size.y/2; //The code here assumes pos is at the bottom center

	GaemTileMap* map = GaemTileMap::instance();

	vec appliedVel = vel;

	bool isOnSlope = map->CollidesWithSlope(pos, appliedVel.y, dt);
	if (isOnSlope) {
		// On a slope, we will override the Y displacement with the X displacement, either upwards or downwards.
		// Pythagoras wouldn't approve that we move at the same velocity on a flat X axis than when move on both X and Y.
		// The mathematically accurate value would be /2 but we don't want to slow the player that much either.
		appliedVel.x = sqrt((vel.x * vel.x) / 1.8f);
		if (vel.x < 0) appliedVel.x = -appliedVel.x;
	}
	vec posf = pos + appliedVel * dt;

	//Debug::out << "----------";
	//Debug::out << abs(pos.x - posf.x) / dt;
	//Debug::out << "slope=" << isOnSlope;

	vec direction = posf - pos;
	const float E = 1;
	float halfWidth = size.x / 2;

	//Skip horizontal collisions when on a slope
	if (isOnSlope) {
		goto horz_exit;
	}

	if (direction.x < 0) //Moving left
	{
		int xo = Tile::ToTiles(pos.x - halfWidth);
		int xn = Tile::ToTiles(posf.x - halfWidth);
		int yTop = Tile::ToTiles(pos.y - size.y + E);
		int yBottom = Tile::ToTiles(pos.y - E);
		for (int x = xo; x >= xn; x--)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				Tile t = map->GetTile(x, y);
				if (t.isFullSolid())
				{
					posf.x = Tile::Right(x) + halfWidth;
					ret.leftWallCollision = t;
					goto horz_exit;
				}
			}
		}
		//No collision left
	}
	else if (direction.x > 0) //Moving right
	{
		int xo = Tile::ToTiles(pos.x + halfWidth);
		int xn = Tile::ToTiles(posf.x + halfWidth);
		int yTop = Tile::ToTiles(pos.y - size.y + E);
		int yBottom = Tile::ToTiles(pos.y - E);
		for (int x = xo; x <= xn; x++)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				Tile t = map->GetTile(x, y);
				if (t.isFullSolid())
				{
					posf.x = Tile::Left(x) - halfWidth;
					ret.rightWallCollision = t;
					goto horz_exit;
				}
			}
		}
		//No collision right
	}


horz_exit:
	pos.x = posf.x;

	// Handle slopes
	float max_movement_into_slope = abs(appliedVel.x * dt) + 1.f;
	int from_y = floor(pos.y - max_movement_into_slope);
	int to_y = (direction.y < 0) ? ceil(pos.y) : ceil(pos.y + max_movement_into_slope); // don't snap to the slope if you are going up so you can jump from the slope
	for (int y = from_y; y < to_y; y++) {
		const float E = 0.0001f; // hack: we want to get to the edge of a tile before stepping onto the next one, hence the epsilon deduced from the integer value.
		veci tilePos = Tile::ToTiles(posf.x, y - E);
		Tile tile = map->GetTile(tilePos);
		if ((isOnSlope && tile.isFullSolid()) || map->IsPosOnSlope(posf.x, y - E))
		{
			posf.y = y - E;
			ret.groundCollisionPos = tilePos;
			ret.groundCollision = tile;
			ret.leftWallCollision = Tile::NONE;
			ret.rightWallCollision = Tile::NONE;
			goto vert_exit;
		}
	}

	if (direction.y < 0) //Moving up
	{
		int yo = Tile::ToTiles(pos.y - size.y); // top edge
		int yn = Tile::ToTiles(posf.y - size.y);
		int xl = Tile::ToTiles(pos.x - halfWidth + E);
		int xr = Tile::ToTiles(pos.x + halfWidth - E);
		for (int y = yo; y >= yn; y--)
		{
			for (int x = xl; x <= xr; x++)
			{
				Tile t = map->GetTile(x, y);
				if (t.isFullSolid())
				{
					posf.y = Tile::Bottom(y) + size.y;
					ret.ceilingCollision = t;
					goto vert_exit;
				}
			}
		}
		//No collision up
	}
	else if (direction.y > 0) //Moving down
	{
		int yo = Tile::ToTiles(pos.y); // bottom edge
		int yn = Tile::ToTiles(posf.y);
		int xl = Tile::ToTiles(pos.x - halfWidth + E);
		int xr = Tile::ToTiles(pos.x + halfWidth - E);
		for (int y = yo; y <= yn; y++)
		{
			for (int x = xl; x <= xr; x++)
			{
				Tile t = map->GetTile(x, y);
				if ((t.isFullSolid()) || (t.isOneWay() && pos.y - 1.f < (y * Tile::Size)))
				{
					posf.y = Tile::Top(y);
					ret.groundCollisionPos = veci(x,y);
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
	vec rounded = (Tile::ToTiles(pos) * Tile::Size);
	vec diff = pos - rounded;
	float targetY = (Tile::Size - diff.x);
	if (targetY < diff.y) {
		pos.y = rounded.y + targetY;
	}
	return pos;
}

inline vec posOnLeftSlope(vec pos) {
	vec rounded = (Tile::ToTiles(pos) * Tile::Size);
	vec diff = pos - rounded;
	float targetY = diff.x;
	if (targetY < diff.y) {
		pos.y = rounded.y + targetY;
	}
	return pos;
}
*/
