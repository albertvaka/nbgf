#include "scene_hellcross.h"

#ifdef _IMGUI
#include "imgui.h"
#endif

#include "camera.h"
#include "window.h"
#include "scene_manager.h"
#include "input.h"
#include "bullet.h"
#include "assets.h"
#include "simplexnoise.h"
#include "bat.h"
#include "debug.h"
#include "collide.h"
#include "rand.h"
#include "fx.h"
#include "oneshotanim.h"
#include "drawall.h"
#include "particles.h"
#include "enemies_by_screen.h"

const float batClusterSize = 22.f;
const float chanceAngryBat = 0.2f;

const int kInitialPlayerHealth = 3;

const Tile SOLID_TILE = Tile::SOLID_SIMPLE;
const Tile BREAKABLE_TILE = Tile::BREAKABLE_SIMPLE;

const static vec map_size = vec(1000, Window::GAME_HEIGHT/Tile::Size);

HellCrossScene::HellCrossScene()
	: map(map_size.x, map_size.y, Assets::spritesheetTexture)
	, lava(BoxBounds(0, map_size.y*16 - 20, map_size.x*16, 200))
{
	Particles::Init();
	ScreenManager::AddScreen(map.BoundsInWorld());
}

HellCrossScene::~HellCrossScene()
{
	ScreenManager::DeleteAllScreens();
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
	Fx::BeforeEnterScene();

	randomSeed = Rand::roll(0, 10000);
	srand(randomSeed);
	RandomizeMap();

	ScreenManager::UpdateCurrentScreen(map.BoundsInWorld().Center());

	player.Reset(vec(160, 160), kInitialPlayerHealth);

	skillTree.Enable(Skill::GUN);
	skillTree.Enable(Skill::WALLJUMP);
	skillTree.Enable(Skill::BREAK);

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

	Debug::out << "seed=" << randomSeed << ", bats=" << Bat::GetAll().size();

	Fx::ScreenTransition::Start(Assets::fadeInDiamondsShader);
	UpdateCamera();
}

void HellCrossScene::ExitScene()
{
	Particles::ClearAll();
	Bullet::DeleteAll();
	Bat::DeleteAll();
	BackgroundOneShotAnim::DeleteAll();
	ForegroundOneShotAnim::DeleteAll();
	destroyedTiles.Clear();
	EnemiesByScreen::AssertEmpty();
}

void HellCrossScene::UpdateCamera() {
	vec camPos = player.GetCameraTargetPos();
	Camera::SetCenter(camPos);
	Camera::ClampCameraTo(map.BoundsInWorld());
}

void HellCrossScene::Update(float dt)
{
	Fx::Update(dt);
	if (Fx::ScreenTransition::IsActive()) {
		return;
	}

	if (Fx::ScreenTransition::IsJustFinished()) {
		if (Fx::ScreenTransition::Current() != &Assets::fadeInDiamondsShader) {
			// This was a death or outro transition: restart scene
			SceneManager::RestartScene();
		}
		return;
	}

	if (player.health <= 0) {
		//Assets::soundDeath.Play();
		vec normalizedPlayerPos = Camera::WorldToScreen(player.CenterPos()) / Camera::InScreenCoords::Size();
		Assets::fadeOutCircleShader.Activate(); // Must be active to set uniforms
		Assets::fadeOutCircleShader.SetUniform("normalizedTarget", normalizedPlayerPos);
		Shader::Deactivate();
		Fx::ScreenTransition::Start(Assets::fadeOutCircleShader);
		return;
	}

	for (ForegroundOneShotAnim* e : ForegroundOneShotAnim::GetAll()) { // Update this first so one-frame anims aren't deleted before they are drawn once
		e->Update(dt);
	}
	ForegroundOneShotAnim::DeleteNotAlive();
	for (BackgroundOneShotAnim* e : BackgroundOneShotAnim::GetAll()) {
		e->Update(dt);
	}
	BackgroundOneShotAnim::DeleteNotAlive();

	player.Update(dt);

	UpdateCamera();

	for (Bullet* e  : Bullet::GetAll()) {
		e->Update(dt);
	}
	Bullet::DeleteNotAlive();

	for (Bat* e : Bat::GetAll()) {
		e->Update(dt);
	}

#ifdef _DEBUG
	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEdit();
	}
#endif

	Bat::DeleteNotAlive();

	Particles::UpdateAll(dt);

	destroyedTiles.Update(dt);

	for (Lava* l : Lava::GetAll()) {
		l->Update(dt);
	}
}

void HellCrossScene::Draw()
{
	Window::Clear(31, 36, 50);

	if (Debug::Draw) {
		SimplexNoise::DebugDraw(Tile::Size, [this](int x, int y) {
			return SimplexNoise().noise(randomSeed + x / batClusterSize, y / batClusterSize);
		});
	}

	DrawAllInOrder(
		&map,
		&destroyedTiles,
		BackgroundOneShotAnim::GetAll(),
		Bat::GetAll(),
		&Particles::bullet,
		Bullet::GetAll(),
		&Particles::dust,
		ForegroundOneShotAnim::GetAll(),
		&player,
		Lava::GetAll()
	);

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
		veci t = Tile::ToTiles(m);
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::Text("Mouse on tile: %d,%d", t.x, t.y);
		ImGui::End();
	}
#endif

	//player.dust.DrawImGUI("Dust");

	Fx::AfterDraw();
}


