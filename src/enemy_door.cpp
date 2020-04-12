#include "enemy_door.h"

#include "tilemap.h"
#include "assets.h"

const float openAnimationTime = 0.4f; //Animation will take twice this time per tile
const int maxHeight = 10;

EnemyDoor::EnemyDoor(const vec& p)
	: Entity(TileMap::alignToTiles(p) - vec(0, Tile::size))
{
	TileMap* map = TileMap::instance();
	auto tilepos = TileMap::toTiles(pos);
	for (int y = 0; y < maxHeight; y++) {
		if (map->isSolid(tilepos.x, tilepos.y + y)) {
			map->setTile(tilepos.x, tilepos.y + y - 1, Tile::SOLID_DOOR_BOTTOM);
			break;
		}
		map->setTile(tilepos.x, tilepos.y + y, Tile::SOLID_DOOR);
	}
}

void EnemyDoor::Update(float dt)
{
	switch (state) {
	case State::CLOSED: {
		enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Entity* e) {
			return !e->alive; 
		}), enemies.end());
		if (enemies.empty()) {
			state = State::OPENING;
		}
	} break;
	case State::OPENING: {
		openingTimer += dt;
		if (openingTimer > openAnimationTime) {
			openingTimer -= openAnimationTime;
			TileMap* map = TileMap::instance();
			auto tilepos = TileMap::toTiles(pos);
			for (int y = 0; y < maxHeight; y++) {
				Tile t = map->getTile(tilepos.x, tilepos.y + y);
				if (t == Tile::SOLID_DOOR_BOTTOM) {
					map->setTile(tilepos.x, tilepos.y + y, Tile::BG_DOOR_OPENING);
					if (y == 0) {
						state = State::OPEN;
					}
					break;
				}
				else if (t == Tile::BG_DOOR_OPENING) {
					map->setTile(tilepos.x, tilepos.y + y, Tile::NONE);
					map->setTile(tilepos.x, tilepos.y + y - 1, Tile::SOLID_DOOR_BOTTOM);
				}
			}
		}
	} break;
	case State::OPEN:
	break;
	}
}

void EnemyDoor::Draw(sf::RenderTarget& window) const
{
	sf::Sprite& spr = Assets::marioSprite;
	spr.setOrigin(0, 0);
	spr.setTextureRect(sf::IntRect(7 * 16, 6 * 16, 16, 16));
	spr.setPosition(pos.x, pos.y);
	window.draw(spr);
}

