#pragma once

#include <vector>
#include "vector.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

class TileMap
{
public:
	void Init(int seed, sf::Vector2i sizes, float unitsPerTile);
	void Draw(sf::Sprite& sprite, sf::RenderTarget& window);

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

	float unitsPerTile;
	sf::Vector2i sizes;
	int width, height;
	std::vector<bool> tiles;
	bool getTile(int i, int j);
};
