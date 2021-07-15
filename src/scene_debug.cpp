#include "scene_debug.h"

#ifdef _IMGUI
#include "imgui.h"
#endif

#include "scene_manager.h"
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
#include "oneshotanim.h"
#include "drawall.h"
#include "particles.h"

const float batClusterSize = 22.f;
const float chanceAngryBat = 0.2f;

const Tile SOLID_TILE = Tile::SOLID_SIMPLE;
const Tile BREAKABLE_TILE = Tile::BREAKABLE_SIMPLE;
const Tile ONEWAY_TILE = Tile::ONEWAY_SIMPLE;

static vec map_size = vec(100, Window::GAME_HEIGHT/Tile::Size);

DebugScene::DebugScene()
	: map(map_size.x, map_size.y, Assets::spritesheetTexture)
{
	Particles::Init();

	skillTree.Enable(Skill::GUN);
	skillTree.Enable(Skill::WALLJUMP);
	skillTree.Enable(Skill::BREAK);

	ScreenManager::instance()->AddScreen(map.BoundsInWorld());
	ScreenManager::instance()->UpdateCurrentScreen(map.BoundsInWorld().Center());
}

void DebugScene::RandomizeMap() {
	const int start_x = 22;
	for (int y = 0; y < map.Height(); y++) {
		for (int x = start_x; x < map.Width(); x++) {
			Tile t = Tile::NONE;
			if (y == 0 || map.GetTile(x, y - 1) == Tile::NONE) { // do not stack solid blocks
				if (x > 0 && map.GetTile(x - 1, y) != Tile::NONE && Rand::rollf() < 0.2f) {
					t = map.GetTile(x - 1, y); // continue previous block type so we create horizontal platforms
				} else if (y != map.Height()-1 && Rand::rollf() < 0.05f) {
					if (Rand::rollf() < 0.33f) {
						t = BREAKABLE_TILE;
					}
					else {
						t = SOLID_TILE;
					}
				}
			}
			map.SetTile(x, y, t);
		}
	}
}

void DebugScene::EnterScene() 
{
	player.Reset(vec(160,200));

	randomSeed = Rand::roll(0, 10000);
	srand(randomSeed);
	RandomizeMap();

	SimplexNoise simplex;
	for (int y = -1; y < map.Height() - 5; y++) { //don't spawn at the bottom rows
		for (int x = 20; x < map.Width(); x += 2) { // don't spawn at the leftmost part of the map where the player starts, don't spawn two bats together
			if (map.GetTile(x, y).isSolid()) {
				float noise = simplex.noise(randomSeed + x / batClusterSize, y / batClusterSize); // returns a number between -1 and 1
				if (y == -1) noise -= 0.66f;
				if (noise > 0.f) {
					bool angry = (Rand::rollf() < chanceAngryBat);
					new Bat(Tile::FromTiles(x,y+2), angry, false);
					map.SetTile(x - 1, y + 1, Tile::NONE);
					map.SetTile(x, y + 1, Tile::NONE);
					map.SetTile(x + 1, y + 1, Tile::NONE);
					map.SetTile(x - 1, y + 2, Tile::NONE);
					map.SetTile(x, y + 2, Tile::NONE);
					map.SetTile(x + 1, y + 2, Tile::NONE);
				}
			}
		}
	}

	veci pos = Tile::ToTiles(player.pos);
	map.SetTile(pos.x - 1, pos.y + 3, Tile::NONE);
	map.SetTile(pos.x + 0, pos.y + 3, Tile::NONE);
	map.SetTile(pos.x + 1, pos.y + 3, Tile::NONE);
	map.SetTile(pos.x - 1, pos.y + 2, Tile::NONE);
	map.SetTile(pos.x + 0, pos.y + 2, Tile::NONE);
	map.SetTile(pos.x + 1, pos.y + 2, Tile::NONE);
	map.SetTile(pos.x - 1, pos.y + 1, Tile::NONE);
	map.SetTile(pos.x + 0, pos.y + 1, Tile::NONE);
	map.SetTile(pos.x + 1, pos.y + 1, Tile::NONE);
	map.SetTile(pos.x - 1, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x + 0, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x + 1, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x + 2, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x + 3, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x + 2, pos.y + 3, Tile::RSLOPE_1);
	map.SetTile(pos.x + 3, pos.y + 3, SOLID_TILE);
	map.SetTile(pos.x + 4, pos.y + 2, SOLID_TILE);
	map.SetTile(pos.x + 3, pos.y + 2, Tile::RSLOPE_1);
	map.SetTile(pos.x + 4, pos.y + 1, Tile::RSLOPE_1);
	map.SetTile(pos.x + 5, pos.y + 0, Tile::RSLOPE_1);
	map.SetTile(pos.x + 6, pos.y - 1, Tile::RSLOPE_1);
	map.SetTile(pos.x + 7, pos.y - 2, Tile::RSLOPE_1);
	map.SetTile(pos.x + 5, pos.y + 1, SOLID_TILE);
	map.SetTile(pos.x + 6, pos.y - 0, SOLID_TILE);
	map.SetTile(pos.x + 7, pos.y - 1, SOLID_TILE);
	map.SetTile(pos.x + 10, pos.y - 2, SOLID_TILE);
	map.SetTile(pos.x + 8, pos.y - 2, SOLID_TILE);
	map.SetTile(pos.x + 9, pos.y - 2, SOLID_TILE);
	map.SetTile(pos.x - 2, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x - 3, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x - 4, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x - 5, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x - 4, pos.y + 3, Tile::LSLOPE_1);
	map.SetTile(pos.x - 5, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x - 4, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x - 3, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x - 2, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x - 1, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x - 0, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x + 1, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x + 2, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x + 3, pos.y + 1, ONEWAY_TILE);
	map.SetTile(pos.x - 5, pos.y + 3, SOLID_TILE);
	map.SetTile(pos.x - 5, pos.y + 2, Tile::LSLOPE_1);
	map.SetTile(pos.x - 6, pos.y + 2, SOLID_TILE);
	map.SetTile(pos.x - 6, pos.y + 1, Tile::LSLOPE_1);
	map.SetTile(pos.x - 7, pos.y + 1, SOLID_TILE);
	map.SetTile(pos.x - 8, pos.y + 1, SOLID_TILE);
	map.SetTile(pos.x - 9, pos.y + 1, SOLID_TILE);
	map.SetTile(pos.x - 10, pos.y + 1, SOLID_TILE);

	Debug::out << "seed=" << randomSeed << ", bats=" << Bat::GetAll().size();

	UpdateCamera();
}

void DebugScene::ExitScene()
{
	Particles::ClearAll();
	Bullet::DeleteAll();
	Bat::DeleteAll();
	OneShotAnim::DeleteAll();
	DebugWalker::DeleteAll();
	destroyedTiles.Clear();
}

void DebugScene::UpdateCamera() {
	vec camPos = player.GetCameraTargetPos();
	Camera::SetCenter(camPos);
	Camera::ClampCameraTo(map.BoundsInWorld());
}

void DebugScene::Update(float dt)
{
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
	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEdit();
	}
#endif

	Bat::DeleteNotAlive();

	Particles::bullet.UpdateParticles(dt);

	destroyedTiles.Update(dt);

}

void DebugScene::Draw()
{
	Window::Clear(90, 180, 50);

	DrawAllInOrder(
		&map,
		&destroyedTiles,
		Bat::GetAll(),
		OneShotAnim::GetAll(),
		&Particles::bullet,
		Bullet::GetAll(),
		&Particles::polvito,
		DebugWalker::GetAll(),
		&player
	);

#ifdef _IMGUI
	{
		ImGui::Begin("debug scene");
		static float dummyWalkerVel = 220;
		ImGui::SliderFloat("Walker Vel", &dummyWalkerVel, -500.f, 500.f);
		if (ImGui::Button("Create Walker")) {
			new DebugWalker(player.pos - vec(0,player.size.y/2), dummyWalkerVel);
		}
		vec m = Mouse::GetPositionInWorld();
		veci t = Tile::ToTiles(m);
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
	
	map.BoundsInWorld().DebugDraw(0,255,0);

	//player.polvito.DrawImGUI("Polvito");
}


