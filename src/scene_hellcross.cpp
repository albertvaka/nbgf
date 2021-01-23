#include "scene_hellcross.h"

#ifdef _IMGUI
#include "imgui.h"
#endif

#include "scene_manager.h"
#include "input.h"
#include "bullet.h"
#include "assets.h"
#include "simplexnoise.h"
#include "bat.h"
#include "debug.h"
#include "camera.h"
#include "collide.h"
#include "rand.h"
#include "fx.h"
#include "oneshotanim.h"
#include "drawall.h"

const float batClusterSize = 22.f;
const float chanceAngryBat = 0.2f;

const Tile SOLID_TILE = Tile::SOLID_SIMPLE;
const Tile BREAKABLE_TILE = Tile::BREAKABLE_SIMPLE;

static vec map_size = vec(1000, Window::GAME_HEIGHT/Tile::size);

HellCrossScene::HellCrossScene()
	: map(map_size.x, map_size.y, Assets::marioTexture)
	, lava(BoxBounds(0, map_size.y*16 - 20, map_size.x*16, 200))
{
	Bullet::InitParticles();

	skillTree.Enable(Skill::GUN);
	skillTree.Enable(Skill::WALLJUMP);
	skillTree.Enable(Skill::BREAK);

	ScreenManager::instance()->AddScreen(map.BoundsInWorld());
	ScreenManager::instance()->UpdateCurrentScreen(map.BoundsInWorld().Center());


}

void HellCrossScene::RandomizeMap() {
	const int start_x = 0;
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

void HellCrossScene::EnterScene() 
{
	player.Reset(vec(160,160));

	randomSeed = Rand::roll(0, 10000);
	srand(randomSeed);
	RandomizeMap();

	for (int y = -1; y < map.Height() - 5; y++) { //don't spawn at the bottom rows
		for (int x = 20; x < map.Width(); x += 2) { // don't spawn at the leftmost part of the map where the player starts, don't spawn two bats together
			if (map.GetTile(x, y).isSolid()) {
				float noise = Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize); // returns a number between -1 and 1
				if (y == -1) noise -= 0.66f;
				if (noise > 0.f) {
					bool angry = (Rand::rollf() < chanceAngryBat);
					new Bat(TileMap::FromTiles(x,y+2), angry, false);
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

	veci pos = map.ToTiles(player.pos);
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

	Debug::out << "seed=" << randomSeed << ", bats=" << Bat::GetAll().size();

	Fx::ScreenTransition::Start(Assets::fadeInDiamondsShader);
	UpdateCamera();
}

void HellCrossScene::ExitScene()
{
	Bullet::particles.Clear();
	Bullet::DeleteAll();
	Bat::DeleteAll();
	OneShotAnim::DeleteAll();
	destroyedTiles.Clear();
}

void HellCrossScene::UpdateCamera() {
	vec camPos = (player.pos* 17 + Mouse::GetPositionInWorld()*2) / 19.f;
	Camera::SetCenter(camPos);
	Camera::ClampCameraTo(map.BoundsInWorld());
}

void HellCrossScene::Update(float dt)
{
	if (Fx::ScreenTransition::IsJustFinished()) {
		if (Fx::ScreenTransition::Current() != &Assets::fadeInDiamondsShader) {
			// This was a death or outro transition: restart scene
			SceneManager::RestartScene();
		}
		return;
	}

	if (player.health <= 0) {
		//Assets::soundDeath.Play();
		vec normalizedPlayerPos = Camera::WorldToScreen(player.Bounds().Center()) / Camera::InScreenCoords::Size();
		Assets::fadeOutCircleShader.Activate(); // Must be active to set uniforms
		Assets::fadeOutCircleShader.SetUniform("normalizedTarget", normalizedPlayerPos);
		Shader::Deactivate();
		Fx::ScreenTransition::Start(Assets::fadeOutCircleShader);
		return;
	}

	player.Update(dt);

	UpdateCamera();

	for (Bullet* e  : Bullet::GetAll()) {
		e->Update(dt);
	}
	Bullet::DeleteNotAlive();

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
		Fx::ScreenTransition::Start(Assets::fadeOutDiamondsShader);
		return;
	}

	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEdit();
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
	Window::Clear(31, 36, 50);

	if (Debug::Draw) {
		Simplex::DebugDraw(Tile::size, [this](int x, int y) {
			return Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize);
		});
	}

	DrawAllInOrder(
		&map,
		&destroyedTiles,
		Bat::GetAll(),
		OneShotAnim::GetAll(),
		&Bullet::particles,
		Bullet::GetAll(),
		&player,
		Lava::GetAll()
	);

	//Bullet::particles.DrawImGUI("BulletTrail");

#ifdef _DEBUG
	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEditDraw();
	}
#endif

	Camera::InScreenCoords::Begin();
	player.DrawGUI();
	Camera::InScreenCoords::End();

#ifdef _IMGUI
	{
		ImGui::Begin("hellcross scene");
		ImGui::InputFloat("y", &player.pos.y, 0.5f, 1.f, 2, ImGuiInputTextFlags_CharsDecimal);
		vec m = Mouse::GetPositionInWorld();
		veci t = map.ToTiles(m);
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::Text("Mouse on tile: %d,%d", t.x, t.y);
		ImGui::End();
	}
#endif

	//player.polvito.DrawImGUI("Polvito");
}


