#include "enemy_door.h"

#include "gaemtilemap.h"
#include "assets.h"
#include "window.h"
#include "screen.h"
#include "debug.h"
#include "fx.h"

const float openAnimationTime = 0.4f; //Animation will take twice this time per tile
const int maxHeight = 10;

std::unordered_map<int, std::vector<EnemyDoor*>> EnemyDoor::ByScreen;

EnemyDoor::EnemyDoor(int saveId, vec p)
	: Entity(Tile::AlignToTiles(p) - vec(0, Tile::Size))
	, saveId(saveId)
{
	SpawnTiles();
	screen = ScreenManager::instance()->FindScreenContaining(p);
	EnemyDoor::ByScreen[screen].push_back(this);
}

EnemyDoor::~EnemyDoor() {
	auto& list = EnemyDoor::ByScreen[screen];
	list.erase(std::find(list.begin(), list.end(), this));
}

void EnemyDoor::Lock() {
	if (state != State::CLOSED) {
		SpawnTiles();
	}
	state = State::LOCKED;
	enemies.clear(); // we will stop checking for !alive enemies, so the pointers might be freed
}

void EnemyDoor::Open(bool skipAnim)
{
	if (skipAnim) {
		while (OpenOneStep());
	} else {
		state = State::OPENING;
		Fx::Screenshake::Start(3.f, vec(1.5f, 1.5f), vec(30.f, 39.f));
		openingTimer = 0.f;
	}
	enemies.clear(); // we will stop checking for !alive enemies, so the pointers might be freed
}

bool EnemyDoor::OpenOneStep() // returns true if there are more steps remaining
{
	if (state == State::OPEN) {
		return false;
	}
	GaemTileMap* map = GaemTileMap::instance();
	auto tilepos = Tile::ToTiles(pos);
	for (int y = 0; y < maxHeight; y++) {
		Tile t = map->GetTile(tilepos.x, tilepos.y + y);
		if (t == Tile::SOLID_DOOR_BOTTOM) {
			map->SetTile(tilepos.x, tilepos.y + y, Tile::BG_DOOR_OPENING);
			if (y == 0) {
				state = State::OPEN;
				return false;
			}
			return true;
		}
		else if (t == Tile::BG_DOOR_OPENING) {
			map->SetTile(tilepos.x, tilepos.y + y, Tile::BG_PLAIN_COLOR);
			map->SetTile(tilepos.x, tilepos.y + y - 1, Tile::SOLID_DOOR_BOTTOM);
			return true;
		}
	}
	Debug::out << "EnemyDoor::OpenOneStep got to maxHeight";
	return false;
}

void EnemyDoor::SpawnTiles()
{
	GaemTileMap* map = GaemTileMap::instance();
	auto tilepos = Tile::ToTiles(pos);
	for (int y = 0; y < maxHeight; y++) {
		Tile t = map->GetTile(tilepos.x, tilepos.y + y);
		if (t.isSolid() && t != Tile::SOLID_DOOR_BOTTOM && t != Tile::SOLID_DOOR) {
			map->SetTile(tilepos.x, tilepos.y + y - 1, Tile::SOLID_DOOR_BOTTOM);
			return;
		}
		map->SetTile(tilepos.x, tilepos.y + y, Tile::SOLID_DOOR);
	}
	Debug::out << "EnemyDoor::SpawnTiles got to maxHeight";
}

void EnemyDoor::Update(float dt)
{
	switch (state) {
	case State::LOCKED:
		break;
	case State::CLOSED: {
		enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Entity* e) {
			return !e->alive; 
		}), enemies.end());
		if (enemies.empty()) {
			Open();
		}
	} break;
	case State::OPENING: {
		openingTimer += dt;
		if (openingTimer > openAnimationTime) {
			openingTimer -= openAnimationTime;
			OpenOneStep();
		}
	} break;
	case State::OPEN:
	break;
	}
}

void EnemyDoor::Draw() const
{
	Window::Draw(Assets::spritesheetTexture, pos)
		.withOrigin(0, 0)
		.withRect({ 7 * 16, 6 * 16, 16, 16 });
}

