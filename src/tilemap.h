#pragma once

#include <vector>
#include "vector.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

struct TileMap
{
	TileMap(sf::Vector2i _sizes, float _unitsPerTile)
		: sizes(_sizes)
		, unitsPerTile(_unitsPerTile)
		, tiles(new bool[_sizes.x * _sizes.y]{})
	{
	}
	void Randomize(int seed);
	void Draw(sf::RenderTarget& window);

	void set(int x, int y, bool col); //set tile collisionable
	bool isColl(sf::Vector2i pos); //is collisionable
	bool isColl(int x, int y); //is collisionable

	sf::Vector2i tilePos(vec pos); //what tile is in world coordinate
	sf::Vector2i tilePos(float x, float y);
	unsigned int tilePosX(float x);
	unsigned int tilePosY(float y);

	float Top(int y);
	float Bottom(int y);

	float Left(int x);
	float Right(int x);

	sf::Vector2i sizes;
	float unitsPerTile;
	bool* tiles;

	bool isCollInWorldCoordinates(vec p) { return isColl(tilePos(p)); }
	bool isCollInWorldCoordinates(float x, float y) { return isColl(tilePos(x,y)); }
};
