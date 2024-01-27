#pragma once

#include "vec.h"
#include "gaemtilemap.h"
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

	vec posf = pos + appliedVel * dt;

	//Debug::out << "----------";
	//Debug::out << abs(pos.x - posf.x) / dt;

	vec direction = posf - pos;
	const float E = 1;
	float halfWidth = size.x / 2;

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
				if (t.isSolid())
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
				if (t.isSolid())
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
				if (t.isSolid())
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
			bool found = false;
			for (int x = xl; x <= xr; x++)
			{
				Tile t = map->GetTile(x, y);
				if (t.isSolid())
				{
					posf.y = Tile::Top(y);
					ret.groundCollisionPos = veci(x,y);
					ret.groundCollision = t;
					found = true;
				}
			}
			if (found) {
				goto vert_exit;
			}

		}
		//No collision down
	}
vert_exit:
	posf.y -= size.y / 2; // The code here assumes the pos is at to bottom center
	ret.pos = posf;
	return ret;
}
