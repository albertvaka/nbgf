#include "scene_debug.h"

#ifdef _IMGUI
#include "imgui.h"
#endif

#include "input.h"
#include "bullet.h"
#include "assets.h"
#include "simplexnoise.h"
#include "debug_walker.h"
#include "bat.h"
#include "debug.h"
#include "camera.h"
#include "collide.h"
#include "rand.h"
#include "fxmanager.h"
#include "oneshotanim.h"
#include "drawall.h"

const float batClusterSize = 22.f;
const float chanceAngryBat = 0.2f;

const Tile SOLID_TILE = Tile::SOLID_SIMPLE;
const Tile BREAKABLE_TILE = Tile::BREAKABLE_SIMPLE;
const Tile ONEWAY_TILE = Tile::ONEWAY_SIMPLE;

static vec map_size = vec(100, Window::GAME_HEIGHT/Tile::size);

static float dummyWalkerVel = 220;

DebugScene::DebugScene()
	: map(map_size.x, map_size.y, Assets::marioTexture)
{
	Bullet::InitParticles();

	skillTree.Enable(Skill::GUN);
	skillTree.Enable(Skill::WALLJUMP);
	skillTree.Enable(Skill::BREAK);

	ScreenManager::instance()->AddScreen(map.boundsInWorld());
	ScreenManager::instance()->UpdateCurrentScreen(map.boundsInWorld().Center());
}

void DebugScene::RandomizeMap() {
	const int start_x = 22;
	for (int y = 0; y < map.height(); y++) {
		for (int x = start_x; x < map.width(); x++) {
			Tile t = Tile::NONE;
			if (y == 0 || map.getTile(x, y - 1) == Tile::NONE) { // do not stack solid blocks
				if (x > 0 && map.getTile(x - 1, y) != Tile::NONE && Rand::rollf() < 0.2f) {
					t = map.getTile(x - 1, y); // continue previous block type so we create horizontal platforms
				} else if (y != map.height()-1 && Rand::rollf() < 0.05f) {
					if (Rand::rollf() < 0.33f) {
						t = BREAKABLE_TILE;
					}
					else {
						t = SOLID_TILE;
					}
				}
			}
			map.setTile(x, y, t);
		}
	}
}

void DebugScene::EnterScene() 
{
	player.Reset(vec(160,200));

	randomSeed = Rand::roll(0, 10000);
	srand(randomSeed);
	RandomizeMap();

	for (int y = -1; y < map.height() - 5; y++) { //don't spawn at the bottom rows
		for (int x = 20; x < map.width(); x += 2) { // don't spawn at the leftmost part of the map where the player starts, don't spawn two bats together
			if (map.getTile(x, y).isSolid()) {
				float noise = Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize); // returns a number between -1 and 1
				if (y == -1) noise -= 0.66f;
				if (noise > 0.f) {
					bool angry = (Rand::rollf() < chanceAngryBat);
					new Bat(TileMap::fromTiles(x,y+2), angry, false);
					map.setTile(x - 1, y + 1, Tile::NONE);
					map.setTile(x, y + 1, Tile::NONE);
					map.setTile(x + 1, y + 1, Tile::NONE);
					map.setTile(x - 1, y + 2, Tile::NONE);
					map.setTile(x, y + 2, Tile::NONE);
					map.setTile(x + 1, y + 2, Tile::NONE);
				}
			}
		}
	}

	veci pos = map.toTiles(player.pos);
	map.setTile(pos.x - 1, pos.y + 3, Tile::NONE);
	map.setTile(pos.x + 0, pos.y + 3, Tile::NONE);
	map.setTile(pos.x + 1, pos.y + 3, Tile::NONE);
	map.setTile(pos.x - 1, pos.y + 2, Tile::NONE);
	map.setTile(pos.x + 0, pos.y + 2, Tile::NONE);
	map.setTile(pos.x + 1, pos.y + 2, Tile::NONE);
	map.setTile(pos.x - 1, pos.y + 1, Tile::NONE);
	map.setTile(pos.x + 0, pos.y + 1, Tile::NONE);
	map.setTile(pos.x + 1, pos.y + 1, Tile::NONE);
	map.setTile(pos.x - 1, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x + 0, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x + 1, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x + 2, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x + 3, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x + 2, pos.y + 3, Tile::RSLOPE_1);
	map.setTile(pos.x + 3, pos.y + 3, SOLID_TILE);
	map.setTile(pos.x + 4, pos.y + 2, SOLID_TILE);
	map.setTile(pos.x + 3, pos.y + 2, Tile::RSLOPE_1);
	map.setTile(pos.x + 4, pos.y + 1, Tile::RSLOPE_1);
	map.setTile(pos.x + 5, pos.y + 0, Tile::RSLOPE_1);
	map.setTile(pos.x + 6, pos.y - 1, Tile::RSLOPE_1);
	map.setTile(pos.x + 7, pos.y - 2, Tile::RSLOPE_1);
	map.setTile(pos.x + 5, pos.y + 1, SOLID_TILE);
	map.setTile(pos.x + 6, pos.y - 0, SOLID_TILE);
	map.setTile(pos.x + 7, pos.y - 1, SOLID_TILE);
	map.setTile(pos.x + 10, pos.y - 2, SOLID_TILE);
	map.setTile(pos.x + 8, pos.y - 2, SOLID_TILE);
	map.setTile(pos.x + 9, pos.y - 2, SOLID_TILE);
	map.setTile(pos.x - 2, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x - 3, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x - 4, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x - 5, pos.y + 4, SOLID_TILE);
	map.setTile(pos.x - 4, pos.y + 3, Tile::LSLOPE_1);
	map.setTile(pos.x - 5, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x - 4, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x - 3, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x - 2, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x - 1, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x - 0, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x + 1, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x + 2, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x + 3, pos.y + 1, ONEWAY_TILE);
	map.setTile(pos.x - 5, pos.y + 3, SOLID_TILE);
	map.setTile(pos.x - 5, pos.y + 2, Tile::LSLOPE_1);
	map.setTile(pos.x - 6, pos.y + 2, SOLID_TILE);
	map.setTile(pos.x - 6, pos.y + 1, Tile::LSLOPE_1);
	map.setTile(pos.x - 7, pos.y + 1, SOLID_TILE);
	map.setTile(pos.x - 8, pos.y + 1, SOLID_TILE);
	map.setTile(pos.x - 9, pos.y + 1, SOLID_TILE);
	map.setTile(pos.x - 10, pos.y + 1, SOLID_TILE);

	Debug::out << "seed=" << randomSeed << ", bats=" << Bat::GetAll().size();

	UpdateCamera();
}

void DebugScene::ExitScene()
{
	Bullet::particles.Clear();
	Bullet::DeleteAll();
	Bat::DeleteAll();
	OneShotAnim::DeleteAll();
	DebugWalker::DeleteAll();
	destroyedTiles.Clear();
}

void DebugScene::UpdateCamera() {
	vec camPos = (player.pos* 17 + Mouse::GetPositionInWorld()*2) / 19.f;
	camPos += FxManager::GetScreenshake();
	Camera::SetCenter(camPos);
	Camera::ClampCameraTo(map.boundsInWorld());
}

void DebugScene::Update(float dt)
{
	FxManager::Update(dt);
	if (FxManager::IsTheWorldStopped()) {
		return;
	}

	player.Update(dt);

	UpdateCamera();

	for (Bullet* e  : Bullet::GetAll()) {
		e->Update(dt);
	}
	Bullet::DeleteNotAlive();

	for (DebugWalker* e : DebugWalker::GetAll()) {
		e->Update(dt);
	}

	for (Bat* e : Bat::GetAll()) {
		e->Update(dt);
	}

	for (OneShotAnim* e : OneShotAnim::GetAll()) {
		e->Update(dt);
	}
	OneShotAnim::DeleteNotAlive();

#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		EnterScene();
		return;
	}

	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEdit();
	}
#endif

	Bat::DeleteNotAlive();

	Bullet::particles.UpdateParticles(dt);

	destroyedTiles.Update(dt);

}

void DebugScene::Draw()
{
	FxManager::BeginDraw();

	Window::Clear(90, 180, 50);

	DrawAllInOrder(
		&map,
		&destroyedTiles,
		Bat::GetAll(),
		OneShotAnim::GetAll(),
		&Bullet::particles,
		Bullet::GetAll(),
		DebugWalker::GetAll(),
		&player
	);

#ifdef _IMGUI
	{
		ImGui::Begin("debug scene");
		ImGui::SliderFloat("Walker Vel", &dummyWalkerVel, -500.f, 500.f);
		if (ImGui::Button("Create Walker")) {
			new DebugWalker(player.pos - vec(0,player.size.y/2), dummyWalkerVel);
		}
		vec m = Mouse::GetPositionInWorld();
		veci t = map.toTiles(m);
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::Text("Mouse on tile: %d,%d", t.x, t.y);
		ImGui::End();
	}
#endif

#ifdef _DEBUG
	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEditDraw();
	}
#endif
	
	FxManager::EndDraw();

	map.boundsInWorld().Draw(0,255,0);

	//player.polvito.DrawImGUI("Polvito");
}


