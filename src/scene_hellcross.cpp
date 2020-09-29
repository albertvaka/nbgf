#include "scene_hellcross.h"

#ifdef _IMGUI
#include "imgui.h"
#endif

#include "input.h"
#include "bullet.h"
#include "assets.h"
#include "simplexnoise.h"
#include "bat.h"
#include "debug.h"
#include "camera.h"
#include "collide.h"
#include "rand.h"
#include "fxmanager.h"

extern float mainClock;

const float batClusterSize = 22.f;
const float chanceAngryBat = 0.2f;

const Tile SOLID_TILE = Tile::SOLID_SIMPLE;
const Tile BREAKABLE_TILE = Tile::BREAKABLE_SIMPLE;
const Tile ONEWAY_TILE = Tile::ONEWAY_SIMPLE;

const bool slope_test = false;

const float introDuration = 1.f;

#ifdef _DEBUG
static int currentPlacingTile = 1;
#endif

static vec map_size = vec(1000, Window::GAME_HEIGHT/Tile::size);

HellCrossScene::HellCrossScene()
	: map(map_size.x, map_size.y, Assets::marioTexture)
	, lava(Bounds(0, map_size.y*16 - 20, map_size.x*16, 200))
{
	Bullet::InitParticles();

	skillTree.Enable(Skill::GUN);
	skillTree.Enable(Skill::WALLJUMP);

	ScreenManager::instance()->AddScreen(map.boundsInWorld());
	ScreenManager::instance()->UpdateCurrentScreen(map.boundsInWorld().Center());


}

void HellCrossScene::RandomizeMap() {
	const int start_x = slope_test? 22 : 0;
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

void HellCrossScene::EnterScene() 
{
	player.Reset(vec(160,160));

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
	if (slope_test) {
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
	}

	Debug::out << "seed=" << randomSeed << ", bats=" << Bat::GetAll().size();

	FxManager::StartIntroTransition(introDuration);
	UpdateCamera();
}

void HellCrossScene::ExitScene()
{
	Bullet::particles.Clear();
	Bullet::DeleteAll();
	Bat::DeleteAll();
	destroyedTiles.Clear();
}

void HellCrossScene::UpdateCamera() {
	vec camPos = (player.pos* 17 + Mouse::GetPositionInWorld()*2) / 19.f;
	camPos += FxManager::GetScreenshake();
	Camera::SetCenter(camPos);
	Camera::ClampCameraTo(map.boundsInWorld());
}

void HellCrossScene::Update(float dt)
{
	FxManager::Update(dt);

	if (FxManager::IsIntroTransitionActive() || FxManager::IsOuttroTransitionActive()) {
		return;
	} else if (FxManager::IsOuttroTransitionDone()) {
		FxManager::ResetOuttroTransitionDone();
		// Restart scene
		ExitScene();
		EnterScene();
		return;
	}

	for (const Lava* l : Lava::GetAll()) {
		if (l->IsInside(player.pos - vec(0,7.f))) {
			player.dead = true;
			player.invencibleTimer = 1;
			player.pos.y += 6 * dt; //sink slowly in the lava
			player.bfgPos.y = -1000;
			player.onWall = JumpMan::ONWALL_NO;
		}
		if (l->IsInside(player.pos - vec(0, 14.f))) {
			FxManager::StartOuttroTransition(introDuration); // Will reset scene after it's done
			return;
		}
	}

	player.Update(dt);

	UpdateCamera();

	for (Bullet* e  : Bullet::GetAll()) {
		e->Update(dt);
	}
	Bullet::DeleteNotAlive();

	for (Bat* e : Bat::GetAll()) {
		e->Update(dt);
		for (Bullet* b : Bullet::GetAll()) {
			if (b->explode) continue;
			if (Collide(e, b)) {
				b->pos = e->pos;
				b->explode = true;
				e->alive = false;
				Bat::AwakeNearbyBats(e->pos);
				break;
			}
		}
		if (!e->alive) continue;
		if (!player.isInvencible()) {
			if (Collide(player.bounds(), e->bounds())) {
				player.takeDamage(e->pos);
			}
		}
	}


#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		FxManager::StartOuttroTransition(introDuration); // Timer to reset scene
		return;
	}
	if (Debug::Draw) {
		if (Mouse::GetScrollWheelMovement() < 0.f) {
			currentPlacingTile -= 1;
			if (currentPlacingTile < 1) currentPlacingTile = magic_enum::enum_count<Tile::Value>() - 1;
		} else if (Mouse::GetScrollWheelMovement() > 0.f) {
			currentPlacingTile += 1;
			if (currentPlacingTile >= int(magic_enum::enum_count<Tile::Value>())) currentPlacingTile = 1;
		}
		if (Input::IsJustPressed(0,LEFT)) currentPlacingTile = Tile::LSLOPE_1;
		if (Input::IsJustPressed(0,RIGHT)) currentPlacingTile = Tile::RSLOPE_1;
		if (Input::IsJustPressed(0,UP)) currentPlacingTile = Tile::ONEWAY_1;
		if (Input::IsJustPressed(0,DOWN)) currentPlacingTile = Tile::SOLID_1;
		bool left = Mouse::IsPressed(Mouse::Button::Left);
		bool right = Mouse::IsPressed(Mouse::Button::Right);
		if (left || right) {
			Tile what_to_set = left ? Tile::Value(currentPlacingTile) : Tile::NONE;
			vec pos = Mouse::GetPositionInWorld();
			veci tile = map.toTiles(pos);
			if (what_to_set == Tile::NONE) {
				map.setTile(tile.x, tile.y, Tile::NONE);
			}
			else if (map.getTile(tile) != what_to_set && map.getTile(tile) != Tile::SOLID_TRANSPARENT) {
				destroyedTiles.toSpawn.emplace_back(tile.x, tile.y, what_to_set, -1.f);
			}
		}
	}
#endif

	Bat::DeleteNotAlive();

	Bullet::particles.UpdateParticles(dt);

	destroyedTiles.Update(dt);

	for (Lava* l : Lava::GetAll()) {
		l->Update(dt);
	}
}

void HellCrossScene::Draw()
{
	FxManager::BeginDraw();

	Window::Clear(31, 36, 50);

	if (Debug::Draw) {
		Simplex::DebugDraw(Tile::size, [this](int x, int y) {
			return Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize);
		});
	}

	map.Draw();
	destroyedTiles.Draw();

	for (const Bat* e : Bat::GetAll()) {
		e->Draw();
		if (Debug::Draw && Camera::GetBounds().Contains(e->pos)) {
			e->DrawBounds();
			e->DrawSenseArea();
		}
	}

	Bullet::particles.Draw();
	//Bullet::particles.DrawImGUI("BulletTrail");

	for (const Bullet* e : Bullet::GetAll()) {
		e->Draw();
		if (Debug::Draw) {
			e->DrawBounds();
		}
	}

	player.Draw();

	for (const Lava* l : Lava::GetAll()) {
		l->Draw();
	}
	
	if (Debug::Draw) {
		player.bounds().Draw();
		//player.pos.Debuggerino(sf::Color::White);
		//player.bounds().Center().Debuggerino(sf::Color::Magenta);
	}

#ifdef _IMGUI
	{
		ImGui::Begin("hellcross scene");
		ImGui::InputFloat("y", &player.pos.y, 0.5f, 1.f, 2, ImGuiInputTextFlags_CharsDecimal);
		vec m = Mouse::GetPositionInWorld();
		veci t = map.toTiles(m);
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::Text("Mouse on tile: %d,%d", t.x, t.y);
		ImGui::End();
	}
#endif

#ifdef _DEBUG
	if (Debug::Draw) {
		vec pos = Camera::GetBounds().TopLeft() + vec(0, 16);
		Bounds(pos, vec(Tile::size, Tile::size)).Draw(0,0,0);
		Window::Draw(Assets::marioTexture, pos)
			.withRect(Tile::tileToTextureRect[currentPlacingTile]);
	}
#endif

	FxManager::EndDraw();

	//player.polvito.DrawImGUI("Polvito");
}


