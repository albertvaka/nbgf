#pragma once

#include "game_data.h"
#include <vector>

enum class TileType
{
	SIGN_GOOD,
	SIGN_BAD,
	WALL,
	BELT_RIGHT,
	BELT_LEFT,
	BELT_UP,
	BELT_DOWN,
	FLOOR,
	ROOMBA_HOME
};

std::vector< std::vector<TileType> > mapita;
std::vector< std::vector<bool> > passable;
std::vector< std::vector<bool> > passableCleaner;

const std::vector< std::string > mapita_inicial =
{
	"XXYYYYYYYXwSXXYYYYYYYXX",
	"XX +     XDDBX       XX",
	"XY XXXXX XAXBX XXXXX XX",
	"X  k   X XAXBX X   g XX",
	"X  XFKFX YACCY XFGFX XX",
	"XX X   X  1 2  X   X XX",
	"XX YXUYY  0 3  YXyYY XX",
	"XX       XXXXX       XX",
	"XX XXXXX X   X XXXmX XX",
	"XX X   l XFRFX X   X XX",
	"XX XFLFX X   r XFMFX YX",
	"XX X   X YYpYY X   X  X",
	"XX YXPYY       YXuYY  X",
	"XX                   XX",
	"XXXXXXXXXXBBBhXXXXXXXXX",
	"XXXXXXXXXXBBBXXXXXXXXXX",
	"XXXXXXXXXXZZZXXXXXXXXXX",
};

TileType TileFromChar(char c)
{
	switch (c)
	{
		case 'h':
			return TileType::SIGN_GOOD;
		case 'w':
			return TileType::SIGN_BAD;
		case 'X':
		{
			return TileType::WALL;
		} break;
		case 'A':
		{
			return TileType::BELT_UP;
		} break;
		case 'S':
		case 'B':
		case 'Z':
		{
			return TileType::BELT_DOWN;
		} break;
		case 'C':
		{
			return TileType::BELT_LEFT;
		} break;
		case 'D':
		{
			return TileType::BELT_RIGHT;
		} break;
		case 'Y':
		{
			return TileType::ROOMBA_HOME;
		} break;

	}
	return TileType::FLOOR;
}

void drawTile(sf::Sprite& sprite, sf::RenderTarget& window, int i, int j)
{
	int time = mainClock.getElapsedTime().asMilliseconds();
	TileType type = mapita[i][j];
	sprite.setPosition(i * TILE_SIZE, j * TILE_SIZE);

	switch (type) {
	case TileType::ROOMBA_HOME:
	{
		sprite.setTextureRect(sf::IntRect(0, 13 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	} break;
	case TileType::SIGN_GOOD:
		sprite.setTextureRect(sf::IntRect(5*16, 10 * 16, 16, 16));
		break;
	case TileType::SIGN_BAD:
		sprite.setTextureRect(sf::IntRect(6 * 16, 10 * 16, 16, 16));
		break;
	case TileType::FLOOR:
		sprite.setTextureRect(sf::IntRect(64, 48, 16, 16));
		break;
	case TileType::WALL:
	{
		sprite.setTextureRect((sf::IntRect(64 + 16, 48, 16, 16)));
	} break;
	case TileType::BELT_DOWN:
		sprite.setTextureRect(Animation::AnimFrame(AnimationType::BELT_RIGHT, time));
		sprite.setOrigin(0, 16);
		sprite.setRotation(90);
		break;
	case TileType::BELT_UP:
		sprite.setTextureRect(Animation::AnimFrame(AnimationType::BELT_RIGHT, time));
		sprite.setOrigin(16, 0);
		sprite.setRotation(-90);
		break;
	case TileType::BELT_LEFT:
		sprite.setTextureRect(Animation::AnimFrame(AnimationType::BELT_RIGHT, time));
		sprite.setOrigin(16, 16);
		sprite.setRotation(180);
		break;
	case TileType::BELT_RIGHT:
		sprite.setTextureRect(Animation::AnimFrame(AnimationType::BELT_RIGHT, time));
		break;
	}
	window.draw(sprite);
	sprite.setRotation(0);
	sprite.setOrigin(0, 0);
}

ExtremityType letraToExtremity(char c)
{
	switch (c)
	{
		case 'G':
		case 'g':
			return ExtremityType::HEAD;
		case 'K':
		case 'k':
			return ExtremityType::LEFT_ARM;
		case 'L':
		case 'l':
			return ExtremityType::LEFT_LEG;
		case 'R':
		case 'r':
			return ExtremityType::RIGHT_LEG;
		case 'M':
		case 'm':
			return ExtremityType::RIGHT_ARM;
	}
	return ExtremityType::BODY; // should not happen
}

void LoadMap()
{

	passable.resize(mapita_inicial[0].size(), std::vector<bool>(mapita_inicial.size()));
	passableCleaner.resize(mapita_inicial[0].size(), std::vector<bool>(mapita_inicial.size()));
	mapita.resize(mapita_inicial[0].size(), std::vector<TileType>(mapita_inicial.size()));

	int x = 0, y = 0;
	for (auto& row : mapita_inicial)
	{
		for (char c : row)
		{
			passable[x][y] = (c < 'A');
			passableCleaner[x][y] = (c < 'E' || c == 'Z' || c == 'u' || c == 'U' || c == 'p' || c == 'P' || c == 'y');
			mapita[x][y] = TileFromChar(c);


			vec pos(16 * x + 8, 16 * y + 8);
			switch (c)
			{
#ifdef _DEBUG
			case '+':
				new Extremity(pos, ExtremityType::HEAD,  RandomExtremityColor());
				new Extremity(pos + vec(16,0), ExtremityType::LEFT_ARM, RandomExtremityColor());
				new Extremity(pos + 2*vec(16,0), ExtremityType::RIGHT_ARM, RandomExtremityColor());
				new Extremity(pos + 3*vec(16,0), ExtremityType::LEFT_LEG, RandomExtremityColor());
				new Extremity(pos + 4*vec(16,0), ExtremityType::RIGHT_LEG, RandomExtremityColor());
			break;
#endif
			//case '0': new Player(0, pos); break;
			//case '1': new Player(1, pos); break;
			//case '2': new Player(2, pos); break;
			case '3': new Player(3, pos); break;
			case 'A': new Cinta(pos, EntityDirection::UP); break;
			case 'B': new Cinta(pos, EntityDirection::DOWN); break;
			case 'T': new Cinta(pos, EntityDirection::DOWN); break;
			case 'C': new Cinta(pos, EntityDirection::LEFT); break;
			case 'D': new Cinta(pos, EntityDirection::RIGHT); break;
			case 'G':
			case 'K':
			case 'L':
			case 'R':
			case 'M':
				new Mesa(pos, letraToExtremity(c));
				break;

			case 'g':
			case 'k':
			case 'l':
			case 'r':
			case 'm':
				new Collector(pos, letraToExtremity(c));
				break;
			case 'S':
			{
				Spawner *s = new Spawner(pos);
				new Detector(vec(pos.x - 16, pos.y + 16), s);
				new Cinta(pos, EntityDirection::DOWN);
			} break;

			case 'U': //Door: Left Arm
			{
				Door* d = new Door(pos, Door::Panel::LEFT_ARM);
				new DoorSensor(d);
				passable[x][y] = true;
			} break;
			case 'y': //Door: Head
			{
				Door* d = new Door(pos, Door::Panel::HEAD);
				new DoorSensor(d);
				passable[x][y] = true;
			} break;
			case 'P': //Door: Left Leg
			{
				Door* d = new Door(pos, Door::Panel::LEFT_LEG);
				new DoorSensor(d);
				passable[x][y] = true;
			} break;
			case 'p': //Door: Right Leg
			{
				Door* d = new Door(pos, Door::Panel::RIGHT_LEG);
				new DoorSensor(d);
				passable[x][y] = true;
			} break;
			case 'u': //Door: Right Arm
			{
				Door* d = new Door(pos, Door::Panel::RIGHT_ARM);
				new DoorSensor(d);
				passable[x][y] = true;
			} break;
			case 'Z':
			{
				new Despawner(pos);
				new Cinta(pos, EntityDirection::DOWN);
			} break;
			case 'Y':
			{
				new CleanerSpawner(pos);
			} break;
			}
			x += 1;
		}
		y += 1;
		x = 0;
	}

}
