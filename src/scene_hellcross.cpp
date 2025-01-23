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
#include "assets_sounds.h"
#include "simplexnoise.h"
#include "bat.h"
#include "health.h"
#include "debug.h"
#include "collide.h"
#include "rand.h"
#include "fx.h"
#include "oneshotanim.h"
#include "drawall.h"
#include "particles.h"
#include "parallax.h"
#include "enemies_by_screen.h"

const float batClusterSize = 22.f;
const float chanceAngryBat = 0.2f;

const int kInitialPlayerHealth = 3;

const Tile SOLID_TILE = Tile::SOLID_SIMPLE;
const Tile BREAKABLE_TILE = Tile::BREAKABLE_SIMPLE;

const static vec map_size = vec(1000, Window::GAME_HEIGHT/Tile::Size);

const static vec playerStartPosition = vec(160, 160);

HellCrossScene::HellCrossScene()
	: map(map_size.x, map_size.y, Assets::spritesheetTexture)
	, lava(BoxBounds(0, map_size.y*16 - 20, map_size.x*16, 200))
	, scoreText(Assets::font_30)
	, bestScoreText(Assets::font_30)
	, save("hellcross, 0")
	, score(0)
	, bestScore(0)
	, parallax(map.BoundsInWorld(), Assets::forestParallaxTextures, 0.f, 1.f, -142.1f)
{
	MusicPlayer::SetVolume(0.3f);
	MusicPlayer::Ensure(Assets::musicHellcross);

	Particles::Init();
	ScreenManager::AddScreen(map.BoundsInWorld());

	if (save.Has("bestscore")) {
		save.StreamGet("bestscore") >> bestScore;
	}
}

void HellCrossScene::UpdateScores() {
	score = std::max(int((player.pos.x - playerStartPosition.x) / 10), score);
	if (score > bestScore) {
		bestScore = score;
	}
	scoreText.SetString("Distance: " + std::to_string(score) + "m");
	bestScoreText.SetString("Best: " + std::to_string(bestScore) + "m");
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
			if ((y != 2) && (y == 0 || map.GetTile(x, y - 1) == Tile::NONE) && (y < 3 || map.GetTile(x, y - 3) == Tile::NONE)) { // do not stack solid blocks, do not create 2-tile-tall gaps
				if (x > 0 && map.GetTile(x - 1, y) != Tile::NONE && Rand::rollf() < 0.33f) {
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

	ScreenManager::UpdateCurrentScreen(map.BoundsInWorld().Center());

	randomSeed = Rand::roll(0, 10000);
	srand(randomSeed);
	RandomizeMap();

	// Clear the starting area
	veci pos = Tile::ToTiles(playerStartPosition);
	for (int x = -2; x <= 1; x++) {
		for (int y = -2; y <= 5; y++) {
			map.SetTile(pos.x + x, pos.y + y, Tile::NONE);
		}
	}
	map.SetTile(pos.x - 2, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x - 1, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x + 0, pos.y + 4, SOLID_TILE);
	map.SetTile(pos.x + 1, pos.y + 4, SOLID_TILE);

	// Spawn bats
	BoxBounds batBounds = map.BoundsInWorld();
	batBounds.height -= 28; // Bats don't swim in lava
	SimplexNoise simplex;
	for (int y = -1; y < map.Height() - 5; y++) { // don't spawn at the bottom rows
		for (int x = 20; x < map.Width(); x += 2) { // don't spawn at the leftmost part of the map where the player starts, don't spawn two bats together
			if (map.GetTile(x, y).isSolid()) {
				float noise = simplex.noise(randomSeed + x / batClusterSize, y / batClusterSize); // returns a number between -1 and 1
				if (y == -1) noise -= 0.66f;
				if (noise > 0.f) {
					bool angry = (Rand::rollf() < chanceAngryBat);
					new Bat(Tile::FromTiles(x,y+2), angry, false, &batBounds);
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

	player.Reset(playerStartPosition, kInitialPlayerHealth);
	skillTree.Enable(Skill::GUN);
	//skillTree.Enable(Skill::WALLJUMP);
	skillTree.Enable(Skill::BREAK);

	score = 0;
	UpdateScores();

	Debug::out << "seed=" << randomSeed << ", bats=" << Bat::GetAll().size();

	Fx::ScreenTransition::Start(Assets::fadeInDiamondsShader);
	UpdateCamera();
	Fx::FreezeImage::SetAlternativeUpdateFnWhileFrozen([this](float dt) {
		UpdateCamera();
	});
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
	Health::DeleteAll();
}

void HellCrossScene::UpdateCamera() {
	vec camPos = player.GetCameraTargetPos();
	Camera::SetCenter(camPos);
	Camera::ClampCameraTo(map.BoundsInWorld());
}

void HellCrossScene::Update(float dt)
{
	Fx::Update(dt);
	if (Fx::FreezeImage::IsFrozen()) {
		// Fx::FreezeImage's alternate update function has already run at this point
		return;
	}
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

	UpdateScores();

	if (player.health <= 0) {
		Assets::soundMegazero.Play();
		save.StreamPut("bestscore") << bestScore;
		save.Save();
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

	for (Health* g : Health::GetAll()) {
		g->Update(dt);
	}
	Health::DeleteNotAlive();

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
		&parallax,
		&map,
		&destroyedTiles,
		BackgroundOneShotAnim::GetAll(),
		&Particles::dust,
		Bat::GetAll(),
		&Particles::bullet,
		Bullet::GetAll(),
		Health::GetAll(),
		&Particles::health,
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
	player.DrawGUI(true);
	Window::Draw(scoreText, vec(8, 28)).withScale(0.25);
	Window::Draw(bestScoreText, vec(8, 40)).withScale(0.25);
	Camera::InScreenCoords::End();

#ifdef _IMGUI
	{
		ImGui::Begin("hellcross scene");
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


