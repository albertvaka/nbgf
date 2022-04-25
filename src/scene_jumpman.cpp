#include <sstream>

#include "scene_jumpman.h"

#include "scene_manager.h"
#include "input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "camera.h"
#include "window.h"
#include "assets.h"
#include "assets_sounds.h"
#include "screen.h"
#include "bullet.h"
#include "enemy_door.h"
#include "parallax.h"
#include "background_bird.h"
#include "bat.h"
#include "miniooy.h"
#include "missile.h"
#include "mantis.h"
#include "bipedal.h"
#include "fx.h"
#include "fireslime.h"
#include "fireshot.h"
#include "ooytear.h"
#include "explosive.h"
#include "lava.h"
#include "savestation.h"
#include "common_enemy.h"
#include "debug.h"
#include "minotaur.h"
#include "collide.h"
#include "rototext.h"
#include "ooy.h"
#include "goomba.h"
#include "flyingalien.h"
#include "drawall.h"
#include "savestate.h"
#include "bigitem.h"
#include "rocketlauncher.h"
#include "drain.h"
#include "dummyentity.h"
#include "health.h"
#include "healthup.h"
#include "particles.h"
#include "tiled_objects_areas.h"
#include "tiled_tilemap.h"
#include "tiled_objects_entities.h"
#include "tiled_objects_screens.h"
#include "tiled_objects_triggers.h"
#include "trigger.h"
#include "cutscene.h"
#include "dialogs.h"
#include "tweeny.h"

extern float mainClock;
extern bool mainClockPaused;

const float kCamSpeed = 2000.f;
const float kCamZoomSpeed = 0.2f;

const int kPlayerInitialHealth = 4;

const char* kSaveStateGameName = "gaem2020";

const auto kLavaFullScreenShader = []() {
	Assets::waveShader.Activate();
	Assets::waveShader.SetUniform("camera", Camera::Center() * Window::GetViewportScale());
	Assets::waveShader.SetUniform("time", mainClock * 10);
};

const auto kPauseFullScreenShader = []() {
	// FIXME: use a better shader
	Assets::tintShader.Activate();
	Assets::tintShader.SetUniform("flashColor", 0.5f, 0.5f, 0.5f, 0.5f);
};

JumpScene::JumpScene(int saveSlot)
	: map(Tiled::TileMap::Size.x, Tiled::TileMap::Size.y, Assets::spritesheetTexture)
	, rotoText(Assets::font_30, Assets::font_30_outline)
	, saveSlot(saveSlot)
{
	Particles::Init();

	for (const BoxBounds& b : Tiled::Areas::parallax_forest) {
		new Parallax(b, Assets::forestParallaxTextures, 0.25f, 1.f, 172.f);
	}

	for (const BoxBounds& b : Tiled::Areas::parallax_island) {
		new Parallax(b, Assets::islandParallaxTextures, 0.f, 0.3f, -88.9f);
	}

	for (const BoxBounds& b : Tiled::Areas::parallax_cave) {
		new Parallax(b, Assets::caveParallaxTextures, 0.4f, 0.65f, -165.f);
	}

	for (const BoxBounds& b : Tiled::Areas::lava_bg) {
		new Parallax(b, Assets::lavaParallaxTextures, 0.3f, 1.f, -410.f);
	}

	for (const BoxBounds& b : Tiled::Areas::parallax_sewer) {
		new Parallax(b, Assets::sewerParallaxTextures, 0.f, 1.f, -142.1f);
	}

	new Parallax(Tiled::Areas::single_parallax_sewer_2, Assets::sewerParallaxTextures, 0.f, 1.f, -82.7f);
	new Parallax(Tiled::Areas::single_parallax_sewer_3, Assets::sewerParallaxTextures, 0.f, 1.f, -150.7f);

	for (const auto& screen : Tiled::Screens::screen) {
		int id = screenManager.AddScreen(screen);
		for (const BoxBounds& b : Tiled::Areas::lava_bg) {
			if (Collide(screen, b)) {
				waveShaderScreens.push_back(id);
			}
		}
	}

}

JumpScene::~JumpScene() {
	Parallax::DeleteAll();
	screenManager.DeleteAllScreens();
}

void JumpScene::SaveGame() const {
	SaveState saveState = SaveState::Open(kSaveStateGameName, saveSlot);
	if (saveState.HasData()) {
		Debug::out << "Overwriting data in slot " << saveSlot;
	}
	saveState.Clear();

	player.SaveGame(saveState);

	for (HealthUp* g : HealthUp::GetAll()) {
		g->SaveGame(saveState);
	}

	for (Shard* g : Shard::GetAll()) {
		g->SaveGame(saveState);
	}

	for (EnemyDoor* g : EnemyDoor::GetAll()) {
		g->SaveGame(saveState);
	}

	for (Explosive* g : Explosive::GetAll()) {
		g->SaveGame(saveState);
	}

	for (Trigger* g : Trigger::GetAll()) {
		g->SaveGame(saveState);
	}

	skillTree.SaveGame(saveState);
	destroyedTiles.SaveGame(saveState);

	saveState.StreamPut("bossdead_bipedal") << (boss_bipedal == nullptr);
	saveState.StreamPut("bossdead_minotaur") << (boss_minotaur == nullptr);
	saveState.StreamPut("bossdead_ooy") << (boss_ooy == nullptr);
	saveState.StreamPut("bossdead_mantis") << (Mantis::GetAll().empty());

	saveState.Save();
}

void JumpScene::LoadGame() {
	SaveState saveState = SaveState::Open(kSaveStateGameName, saveSlot);
	if (!saveState.HasData()) {
		Debug::out << "No data to load in slot " << saveSlot;
		return;
	}

	for (HealthUp* g : HealthUp::GetAll()) {
		g->LoadGame(saveState);
	}

	for (Shard* g : Shard::GetAll()) {
		g->LoadGame(saveState);
	}

	for (EnemyDoor* g : EnemyDoor::GetAll()) {
		g->LoadGame(saveState);
	}

	for (Explosive* g : Explosive::GetAll()) {
		g->LoadGame(saveState);
	}

	for (Trigger* g : Trigger::GetAll()) {
		g->LoadGame(saveState);
	}

	destroyedTiles.LoadGame(saveState);
	skillTree.LoadGame(saveState);

	for (BigItem* g : BigItem::GetAll()) {
		if (skillTree.IsEnabled(g->skill)) {
			TriggerPickupItem(g, true);
			g->alive = false;
		}
	}

	bool bossdead_bipedal = false;
	saveState.StreamGet("bossdead_bipedal") >> bossdead_bipedal;
	if (bossdead_bipedal && boss_bipedal) {
		boss_bipedal->alive = false;
	}

	bool bossdead_minotaur = false;
	saveState.StreamGet("bossdead_minotaur") >> bossdead_minotaur;
	if (bossdead_minotaur && boss_minotaur) {
		boss_minotaur->alive = false;
	}

	bool bossdead_ooy = false;
	saveState.StreamGet("bossdead_ooy") >> bossdead_ooy;
	if (bossdead_ooy) {
		boss_ooy->alive = false;
	}

	bool bossdead_mantis = false;
	saveState.StreamGet("bossdead_mantis") >> bossdead_mantis;
	if (bossdead_mantis) {
		for (auto m : Mantis::GetAll()) {
			m->alive = false;
		}
	}

	player.LoadGame(saveState);

	Camera::SetCenter(player.GetCameraTargetPos()); // Needed so Update doesn't return because we are on a "camera transition"
	Update(0); //Hackish way to make sure the entities with alive=false trigger things on other components before being deleted
}

void JumpScene::TriggerPickupItem(BigItem* g, [[maybe_unused]] bool fromSave) {

	switch (g->skill) {
	case Skill::WALLJUMP:
	{
	}
	break;
	case Skill::ATTACK:
	{
		int screen_gun = screenManager.FindScreenContaining(g->pos);
		for (Bat* e : Bat::GetAll()) {
			if (e->screen == screen_gun) {
				e->awakened = true;
			}
		}
		for (auto const& [id, pos] : Tiled::Entities::initial_batawake) {
			Bat* b = new Bat(pos, false, true);
			for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
				s->AddEnemy(b);
			}
		}
	}
	break;
	default:
		break;
	}
}

void JumpScene::EnterScene()
{
	Fx::BeforeEnterScene();

	player.Reset(Tiled::Entities::single_spawn, kPlayerInitialHealth);
	skillTree.Reset();


	screenManager.UpdateCurrentScreen(Tiled::Entities::single_initial_screen);


	map.LoadFromTiled<Tiled::TileMap>();

	new BigItem(Tiled::Entities::single_skill_walljump, Skill::WALLJUMP);
	new BigItem(Tiled::Entities::single_skill_gun, Skill::GUN);
	new BigItem(Tiled::Entities::single_skill_breakblocks, Skill::BREAK);
	new BigItem(Tiled::Entities::single_skill_attack, Skill::ATTACK);
	new BigItem(Tiled::Entities::single_skill_dive, Skill::DIVE);
	new BigItem(Tiled::Entities::single_skill_dash, Skill::DASH);

	// FIXME: NPC
	(new ForegroundOneShotAnim(Assets::warriorTexture, Tiled::Entities::single_npc, AnimLib::NPC_IDLE, 1.2f))->anim.loopable = true;

	for (auto const& [id, pos] : Tiled::Entities::save) {
		new SaveStation(id, pos);
	}

	for (auto const& [id, pos] : Tiled::Entities::enemy_door) {
		EnemyDoor* d = new EnemyDoor(id, pos);
		screenManager.FindScreenContaining(d->pos);
	}

	for (auto const& [id, pos] : Tiled::Entities::bat) {
		Bat* b = new Bat(pos, false, false);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::angrybat) {
		Bat* b = new Bat(pos, true, false);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::angrybatawake) {
		Bat* b = new Bat(pos, true, true);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::batawake) {
		Bat* b =new Bat(pos, false, true);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::fireslime) {
		auto b = new FireSlime(pos);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::rocket_launcher) {
		new RocketLauncher(pos);
	}

	for (auto const& [id, pos] : Tiled::Entities::drain) {
		new Drain(pos);
	}

	for (auto const& [id, pos] : Tiled::Entities::miniooy) {
		auto b = new MiniOoy(pos);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::goomba) {
		auto b = new Goomba(pos, Goomba::Type::WALKER);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::goombacharger) {
		auto b = new Goomba(pos, Goomba::Type::CHARGER);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::goombashielder) {
		auto b = new Goomba(pos, Goomba::Type::SHIELDER);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::mantis) {
		auto b = new Mantis(pos);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
		for (SaveStation* s : SaveStation::ByScreen[b->screen]) {
			s->AddHiddenBy(b);
		}
	}

	for (auto const& [id, pos] : Tiled::Entities::flyingalien) {
		auto b = new FlyingAlien(pos);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	Bipedal* bipedal = new Bipedal(Tiled::Entities::single_boss_bipedal);
	for (EnemyDoor* s : EnemyDoor::ByScreen[bipedal->screen]) {
		s->AddEnemy(bipedal);
	}
	boss_bipedal = bipedal;


	Minotaur* minotaur = new Minotaur(Tiled::Entities::single_boss_minotaur);
	for (EnemyDoor* s : EnemyDoor::ByScreen[minotaur->screen]) {
		s->AddEnemy(minotaur);
	}
	boss_minotaur = minotaur;

	Ooy* ooy = new Ooy(Tiled::Entities::single_ooy);
	for (EnemyDoor* s : EnemyDoor::ByScreen[ooy->screen]) {
		s->AddEnemy(ooy);
	}
	boss_ooy = ooy;

	/*
	for (auto const& [id, pos] : Tiled::Entities::minotaur) {
		auto b = new Minotaur(pos);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}
	*/

	for (SaveStation* s : SaveStation::ByScreen[screenManager.FindScreenContaining(boss_bipedal->pos)]) {
		s->AddHiddenBy(boss_bipedal);
	}

	for (auto const& [id, pos] : Tiled::Entities::healthup) {
		new HealthUp(id, pos);
	}

	for (auto const& [id, pos] : Tiled::Entities::shard) {
		new Shard(id, pos);
	}

	for (auto const& [id, pos] : Tiled::Entities::explosive) {
		new Explosive(id, pos, false);
	}

	for (auto const& [id, pos] : Tiled::Entities::temp_explosive) {
		new Explosive(id, pos, true);
	}

	for (const BoxBounds& a : Tiled::Areas::bg_bird_spawner) {
		new BackgroundBirdSpawner(a);
	}

	for (const BoxBounds& a : Tiled::Areas::lava) {
		Lava* lava = new Lava(a);
		if (a.Contains(Tiled::Entities::single_lava_initial_height)) {
			raising_lava = lava;
			raising_lava_target_height = lava->CurrentLevel();
			lava->SetLevel(Tiled::Entities::single_lava_initial_height.y, true);
		}
	}

	new Trigger("lava_raise", Tiled::Triggers::single_trigger_lava, [this](Trigger* t, bool isLoadingSave) {
		raising_lava->SetLevel(raising_lava_target_height, isLoadingSave);
	});

	new Trigger("lava_raise", Tiled::Triggers::single_trigger_fast_lava, [this](Trigger* t, bool isLoadingSave) {
		raising_lava->SetRaiseSpeed(Lava::kFastRaiseSpeed);
	});

	new Trigger(Tiled::Triggers::single_trigger_leafs, [](Trigger* t) {
		Particles::leafs.pos = JumpMan::instance()->pos;
		Particles::leafs.AddParticles(17);
		Particles::leafs.FlipX();
		Particles::leafs.AddParticles(17);
		for (auto& p : Particles::leafs.particles) {
			p.pos.y += Rand::roll(-30, 30) - 48;
			p.pos.x += Rand::roll(-18, 18);
		}
		Assets::soundLeaves.Play();
		Assets::soundLeaves[0].Play();
		Assets::soundLeaves[1].Play();
	});

	DummyEntity* fallingRock1 = new DummyEntity(AnimLib::BIG_ROCK, Tiled::Entities::single_rocks_origin_1);
	DummyEntity* fallingRock2 = new DummyEntity(AnimLib::BIG_ROCK, Tiled::Entities::single_rocks_origin_2);
	DummyEntity* fallingRock3 = new DummyEntity(AnimLib::BIG_ROCK, Tiled::Entities::single_rocks_origin_3);
	if (boss_ooy) {
		boss_ooy->state = Ooy::State::STILL;
	}
	new Trigger("rockfall", Tiled::Triggers::single_trigger_rockfall, [this, fallingRock1, fallingRock2, fallingRock3](Trigger* t, bool isLoadingSave) {

		if (isLoadingSave) {
			fallingRock1->SetTransform(Tiled::Entities::single_rocks_target_1);
			fallingRock2->SetTransform(Tiled::Entities::single_rocks_target_2);
			fallingRock3->SetTransform(Tiled::Entities::single_rocks_target_3);
			map.SetTile(Tile::ToTiles(Tiled::Entities::single_rocks_target_1), Tile::SOLID_TRANSPARENT);
			map.SetTile(Tile::ToTiles(Tiled::Entities::single_rocks_target_2), Tile::SOLID_TRANSPARENT);
			map.SetTile(Tile::ToTiles(Tiled::Entities::single_rocks_target_3), Tile::SOLID_TRANSPARENT);
			if (boss_ooy) boss_ooy->state = Ooy::State::EXIT_CHASE;
			return;
		}

		player.pos.x -= 100.f;
		SaveGame();
		player.pos.x += 100.f;


		CutSceneBuilder()
		.PlayOneFrame([]() {
			//FIXME: Replace by calling a special "ragdoll update" function on the player during the cutscene
			//that ignores input but keeps the player's current momentum until it stops naturally
			Input::IgnoreInput(true);
		})
		.WaitAndThen()
		.PlayOneFrame([]() {
			Fx::Screenshake::Start(3.5f, vec(0.6f, 0.6f), vec(35.f, 45.f));
			Fx::Screenshake::screenshakeDampening = 1.03f;
		})
		.DoNothingFor(0.35f)
		.WaitAndThen()
		.PlayOneFrame([this]() {
			player.lookingLeft = true;
		})
		.DoNothingFor(0.4f)
		.WaitAndThen()
		.PlayOneFrame([]() {
			Fx::Screenshake::screenshakeDampening = -1.f;
		})
		.DoNothingFor(2.5f)
		.WaitAndThen()
		.PlayOneFrame([]() {
			Fx::Screenshake::screenshakeDampening = 0.9f;
		})
		.DoNothingFor(1.0f)
		.WaitAndThen()
		.PlayOneFrame([this]() {
			map.SetTile(Tile::ToTiles(Tiled::Entities::single_rocks_target_1), Tile::SOLID_TRANSPARENT);
			map.SetTile(Tile::ToTiles(Tiled::Entities::single_rocks_target_2), Tile::SOLID_TRANSPARENT);
			map.SetTile(Tile::ToTiles(Tiled::Entities::single_rocks_target_3), Tile::SOLID_TRANSPARENT);
			player.lookingLeft = false;
			Input::IgnoreInput(false);
			if (boss_ooy) boss_ooy->state = Ooy::State::EXIT_CHASE;
		});

		CutSceneBuilder()
		.DoNothingFor(0.9f)
		.WaitAndThen()
		.Play(0.7f, [fallingRock1](float progress) {
			fallingRock1->SetTransform(tweeny::easing::quadraticIn.run(progress, Tiled::Entities::single_rocks_origin_1, Tiled::Entities::single_rocks_middle_1));
		})
		.WaitAndThen()
		.Play(1.0f, [fallingRock1](float progress) {
			fallingRock1->SetTransform(tweeny::easing::quadraticIn.run(progress, Tiled::Entities::single_rocks_middle_1, Tiled::Entities::single_rocks_target_1));
		});

		CutSceneBuilder()
		.DoNothingFor(1.1f)
		.WaitAndThen()
		.Play(1.0f, [fallingRock2](float progress) {
			fallingRock2->SetTransform(tweeny::easing::quadraticIn.run(progress, Tiled::Entities::single_rocks_origin_2, Tiled::Entities::single_rocks_middle_2));
		})
		.WaitAndThen()
		.Play(0.6f, [fallingRock2](float progress) {
			fallingRock2->SetTransform(tweeny::easing::quadraticIn.run(progress, Tiled::Entities::single_rocks_middle_2, Tiled::Entities::single_rocks_target_2));
		});

		CutSceneBuilder()
		.DoNothingFor(1.4f)
		.WaitAndThen()
		.Play(0.6f, [fallingRock3](float progress) {
			fallingRock3->SetTransform(tweeny::easing::cubicIn.run(progress, Tiled::Entities::single_rocks_origin_3, Tiled::Entities::single_rocks_middle_3));
		})
		.WaitAndThen()
		.Play(0.5f, [fallingRock3](float progress) {
			fallingRock3->SetTransform(tweeny::easing::quadraticIn.run(progress, Tiled::Entities::single_rocks_middle_3, Tiled::Entities::single_rocks_middle_bounce_3));
		})
		.WaitAndThen()
		.Play(0.5f, [fallingRock3](float progress) {
			fallingRock3->SetTransform(tweeny::easing::quadraticIn.run(progress, Tiled::Entities::single_rocks_middle_bounce_3, Tiled::Entities::single_rocks_target_3));
		});

	});

	Camera::SetCenter(player.GetCameraTargetPos());

	LoadGame();

	zoneManager.Reset();

	Fx::FreezeImage::SetAlternativeUpdateFnWhileFrozen([this](float dt){
		UpdateCamera(dt);
	});

	Fx::ScreenTransition::Start(Assets::fadeInDiamondsShader);
}

bool JumpScene::UpdateCamera(float dt) {
	zoneManager.Update(dt);

	if (Debug::CameraFixed) return false;

	float camZoom = Fx::FreezeImage::IsFrozen() && player.justHit ? 1.5f : 1.f;
	float oldZoom = Camera::Zoom();
	float zoomChange = camZoom - oldZoom;
	Mates::Clamp(zoomChange, -kCamZoomSpeed *dt, kCamZoomSpeed *dt);
	Camera::SetZoom(oldZoom + zoomChange);

	vec camPos = player.GetCameraTargetPos();
	vec oldPos = Camera::Center();
	vec displacement = camPos - oldPos;
	bool inScreenTransition = displacement.Truncate(kCamSpeed * dt);
	Camera::SetCenter(oldPos + displacement);
	return inScreenTransition;
}

void JumpScene::ExitScene()
{
	Trigger::DeleteAll();
	CutScene::DeleteAll();
	Particles::ClearAll();
	Bullet::DeleteAll();
	Missile::DeleteAll();
	FireShot::DeleteAll();
	OoyTear::DeleteAll();
	Bat::DeleteAll();
	MiniOoy::DeleteAll();
	BackgroundBirdSpawner::DeleteAll();
	BackgroundBird::DeleteAll();
	Goomba::DeleteAll();
	Ooy::DeleteAll();
	DummyEntity::DeleteAll();
	RocketLauncher::DeleteAll();
	Drain::DeleteAll();
	Minotaur::DeleteAll();
	Mantis::DeleteAll();
	FlyingAlien::DeleteAll();
	FireSlime::DeleteAll();
	BackgroundOneShotAnim::DeleteAll();
	ForegroundOneShotAnim::DeleteAll();
	Bipedal::DeleteAll();
	Lava::DeleteAll();
	destroyedTiles.Clear();
	EnemyDoor::DeleteAll();
	BigItem::DeleteAll();
	HealthUp::DeleteAll();
	Shard::DeleteAll();
	Explosive::DeleteAll();
	Health::DeleteAll();
	SaveStation::DeleteAll();
}

void JumpScene::Update(float dt)
{
#ifdef _DEBUG
	if (Debug::FrameByFrame && Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		player.pos = Camera::Center() + vec(0, 16);
	}
#endif

	if (playerPaused) {
		mainClockPaused = true;
	} else {
		Fx::Update(dt);
		bool frozen = Fx::FreezeImage::IsFrozen();
		mainClockPaused = frozen;
		if (frozen) {
			// Fx::FreezeImage's alternate update function has already run at this point
			// FIXME: Get rid of the whole generic "frozen alternate update" thing and handle it here
			return;
		}
		if (Fx::ScreenTransition::IsActive()) {
			return;
		}
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

	contextActionButton = false;

	for (ForegroundOneShotAnim* e : ForegroundOneShotAnim::GetAll()) { // Update this first so one-frame anims aren't deleted before they are drawn once
		e->Update(dt);
	}
	ForegroundOneShotAnim::DeleteNotAlive();
	for (BackgroundOneShotAnim* e : BackgroundOneShotAnim::GetAll()) {
		e->Update(dt);
	}
	BackgroundOneShotAnim::DeleteNotAlive();

	dialogDriver.Update(dt);
	if (dialogDriver.IsOpen()) {
		return;
	}

	if (Input::IsJustPressedAnyPlayer(GameKeys::PAUSE)) {
		playerPaused = !playerPaused;

		if (playerPaused) {
			Fx::FullscreenShader::SetShader(kPauseFullScreenShader);
		}
		else {
			if (shaderLavaActive) {
				Fx::FullscreenShader::SetShader(kLavaFullScreenShader);
			}
			else {
				Fx::FullscreenShader::SetShader(nullptr);
			}
		}
	}

	if (playerPaused) {
		return;
	}

	bool cutScenePausingGame = false;
	for (CutScene* e : CutScene::GetAll()) {
		e->Update(dt);
		cutScenePausingGame = cutScenePausingGame || e->pauseGame;
	}
	CutScene::DeleteNotAlive();
	if (cutScenePausingGame) {
		return;
	}

	player.Update(dt);

	for (Trigger* e : Trigger::GetAll()) {
		e->Update(dt);
	}

	screenManager.UpdateCurrentScreen(player.pos);

	bool inScreenTransition = UpdateCamera(dt);
	if (inScreenTransition) {
		return;
	}

	for (BackgroundBirdSpawner* e : BackgroundBirdSpawner::GetAll()) {
		e->Update(dt);
	}
	BackgroundBird::DeleteNotAlive();
	for (BackgroundBird* e : BackgroundBird::GetAll()) {
		e->Update(dt);
	}

	for (RocketLauncher* e : RocketLauncher::GetAll()) {
		e->Update(dt);
	}

	for (Drain* e : Drain::GetAll()) {
		e->Update(dt);
	}

	for (Bipedal* e : Bipedal::GetAll()) {
		e->Update(dt);
	}

	for (Bullet* e  : Bullet::GetAll()) {
		e->Update(dt);
	}
	Bullet::DeleteNotAlive();

	for (FireShot* e : FireShot::GetAll()) {
		e->Update(dt);
	}
	FireShot::DeleteNotAlive();

	for (OoyTear* e : OoyTear::GetAll()) {
		e->Update(dt);
	}
	OoyTear::DeleteNotAlive();

	for (Missile* e  : Missile::GetAll()) {
		e->Update(dt);
	}
	Missile::DeleteNotAlive();

	for (Bat* e : Bat::GetAll()) {
		e->Update(dt);
	}

	for (MiniOoy* e : MiniOoy::GetAll()) {
		e->Update(dt);
	}

	for (Goomba* e : Goomba::GetAll()) {
		e->Update(dt);
	}

	for (Ooy* e : Ooy::GetAll()) {
		e->Update(dt);
	}

	for (Minotaur* e : Minotaur::GetAll()) {
		e->Update(dt);
	}

	Mantis::SelfCollide();
	for (Mantis* e : Mantis::GetAll()) {
		e->Update(dt);
	}

	for (FlyingAlien* e : FlyingAlien::GetAll()) {
		e->Update(dt);
	}

	for (FireSlime* e : FireSlime::GetAll()) {
		e->Update(dt);
	}

#ifdef _DEBUG
	const SDL_Scancode killall = SDL_SCANCODE_F11;
	const SDL_Scancode unlockbasics = SDL_SCANCODE_F8;
	const SDL_Scancode teleport = SDL_SCANCODE_F9;
	const SDL_Scancode screen_left = SDL_SCANCODE_F6;
	const SDL_Scancode screen_right = SDL_SCANCODE_F7;
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		// actual restart is done in main.cpp, this is here only to clear the save
		if (Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
			SaveState saveState = SaveState::Open(kSaveStateGameName, saveSlot);
			saveState.Clear();
			saveState.Save();
		}
	}
	if (Keyboard::IsKeyJustPressed(teleport)) {
		player.pos = Tiled::Entities::single_debug_teleport;
		screenManager.UpdateCurrentScreen(player.pos);
		Camera::SetCenter(player.GetCameraTargetPos());
	}
	if (Keyboard::IsKeyJustPressed(unlockbasics)) {
		skillTree.Enable(Skill::ATTACK);
		skillTree.Enable(Skill::DIVE);
		skillTree.Enable(Skill::DASH);
		skillTree.Enable(Skill::WALLJUMP);
		skillTree.Enable(Skill::BREAK);
		if (Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
			skillTree.Enable(Skill::GUN);
		}
	}
	if (Keyboard::IsKeyJustPressed(screen_left)) {
		player.pos.x -= Window::GAME_WIDTH;
	}
	if (Keyboard::IsKeyJustPressed(screen_right)) {
		player.pos.x += Window::GAME_WIDTH;
	}
	if (Keyboard::IsKeyJustPressed(killall)) {
		for (Bat* e : Bat::GetAll()) {
			if (e->screen == screenManager.CurrentScreen()) {
				DieWithSmallExplosion(e);
			}
		}
		for (FireSlime* e : FireSlime::GetAll()) {
			if (e->screen == screenManager.CurrentScreen()) {
				DieWithSmallExplosion(e);
			}
		}
	}

	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEdit();
	}
#endif

	for (EnemyDoor* ed : EnemyDoor::GetAll()) {
		// If clossed, it checks for enemies with alive == false to open
		ed->Update(dt);
	}

	for (SaveStation* ss : SaveStation::GetAll()) {
		// If hidden, it checks for enemies with alive == false to unhide
		if (ss->Update(dt)) { // true if player can interact with it
			contextActionButton = true;
			if (Input::IsJustPressed(0, GameKeys::ACTION)) {
				// TODO: Interaction animation
				SaveGame();
				// Exit and Enter the scene again, resetting the state of everything and loading the state we just saved
				Fx::ScreenTransition::Start(Assets::fadeOutDiamondsShader);
			}
		}
	}

	if (boss_bipedal && !boss_bipedal->alive) {
		boss_bipedal = nullptr;
	}
	if (boss_minotaur && !boss_minotaur->alive) {
		boss_minotaur = nullptr;
	}
	if (boss_ooy && !boss_ooy->alive) {
		boss_ooy = nullptr;
	}

	// Delete enemies after updating doors and savestations that might check for them
	Bat::DeleteNotAlive();
	Minotaur::DeleteNotAlive();
	MiniOoy::DeleteNotAlive();
	Goomba::DeleteNotAlive();
	Ooy::DeleteNotAlive();
	DummyEntity::DeleteNotAlive();
	FlyingAlien::DeleteNotAlive();
	Mantis::DeleteNotAlive();
	FireSlime::DeleteNotAlive();
	Bipedal::DeleteNotAlive();

	for (HealthUp* g : HealthUp::GetAll()) {
		g->Update(dt);
	}

	for (Shard* g : Shard::GetAll()) {
		g->Update(dt);
	}

	for (Health* g : Health::GetAll()) {
		g->Update(dt);
	}
	Health::DeleteNotAlive();

	for (BigItem* g : BigItem::GetAll()) {
		g->Update(dt);
		if (Collide(g->Bounds(), player.CollisionBounds())) {
			skillTree.Enable(g->skill);
			
			//TODO: Pickup animation or popup or something
			switch(g->skill) {
				case Skill::WALLJUMP:
					rotoText.ShowMessage("Walljumperino");
					break;
				case Skill::ATTACK:
					rotoText.ShowMessage("Sworderino!");
					break;
				case Skill::DIVE:
					rotoText.ShowMessage("Stab downerino!");
					break;
				case Skill::DASH:
					rotoText.ShowMessage("Dasherino");
					break;
				case Skill::GUN:
					rotoText.ShowMessage("Big F. Gun");
					break;
				case Skill::BREAK:
					rotoText.ShowMessage("Your sword can\nnow break stuff!");
					break;
				default:
					break;
			}

			TriggerPickupItem(g, false);
			
			g->alive = false;

			SaveGame(); // silently save
		}
	}
	BigItem::DeleteNotAlive();

	Particles::UpdateAll(dt);

	destroyedTiles.Update(dt);

	for (Lava* l : Lava::GetAll()) {
		l->Update(dt);
	}

	rotoText.Update(dt);

	// Enable waves shader in lava cave
	bool inLavaCave = false;
	for (int screen : waveShaderScreens) {
		if (screen == screenManager.CurrentScreen()) {
			inLavaCave = true;
			break;
		}
	}
	if (inLavaCave && !shaderLavaActive) {
		shaderLavaActive = true;
		Fx::FullscreenShader::SetShader(kLavaFullScreenShader);
	}
	else if (!inLavaCave && shaderLavaActive) {
		shaderLavaActive = false;
		Fx::FullscreenShader::SetShader(nullptr);
	}
}

void JumpScene::Draw()
{
	const SDL_Color& bgColor = zoneManager.GetBgColor();
	Window::Clear(bgColor);
	bool isFullScreenshaderActive = Fx::FullscreenShader::Activate();
	if (isFullScreenshaderActive) { // Clear again, this time for the fullscreen texture
		Window::Clear(bgColor);
	}

	for (const Parallax* g : Parallax::GetAll()) {
		g->Draw();
	}

	for (const BackgroundBird* g : BackgroundBird::GetAll()) {
		g->Draw();
	}

	map.Draw();

	for (const BigItem* g : BigItem::GetAll()) {
		g->DrawPedestal();
	}

	destroyedTiles.Draw();

	//Particles::ooyTearTrail.DrawImGUI("Tears");

	DrawAllInOrder(
		SaveStation::GetAll(),
		EnemyDoor::GetAll(),
		BackgroundOneShotAnim::GetAll(),
		DummyEntity::GetAll(),
		Goomba::GetAll(),
		MiniOoy::GetAll(),
		&Particles::ooyTearTrail,
		Ooy::GetAll(),
		Minotaur::GetAll(),
		FlyingAlien::GetAll(),
		FireSlime::GetAll(),
		Bat::GetAll(),
		Bipedal::GetAll(),
		&Particles::dust,
		Mantis::GetAll(),
		RocketLauncher::GetAll(),
		Drain::GetAll(),
		&Particles::leafs,
		&Particles::bullet,
		&Particles::missile,
		ForegroundOneShotAnim::GetAll(),
		Health::GetAll(),
		&Particles::health,
		Bullet::GetAll(),
		FireShot::GetAll(),
		OoyTear::GetAll(),
		Missile::GetAll(),
		HealthUp::GetAll(),
		Shard::GetAll(),
		BigItem::GetAll(),
		&Particles::itemSparks,
		&player,
		Lava::GetAll(),
		Trigger::GetAll()
	);

	//Particles::leafs.DrawImGUI();

	if (contextActionButton) {
		Window::Draw(Assets::spritesheetTexture, player.CollisionBounds().TopRight() + vec(2, -6))
			.withRect(Animation::GetRectAtTime(AnimLib::BUTTON_B_PRESS, mainClock));
	}

	rotoText.Draw(vec(0, -30));

#ifdef _IMGUI
	{
		ImGui::Begin("scene");
		vec m = Mouse::GetPositionInWorld();
		veci t = Tile::ToTiles(m);
		ImGui::Text("mainclock: %f", mainClock);
		ImGui::Text("Mouse: %f,%f Tile: %d,%d", m.x, m.y, t.x, t.y);
		if (ImGui::Button("Start NPC dialog")) {
			dialogDriver.StartDialog(dialogWithRandomNpc);
		}

		static bool placingDummy = false;
		if (ImGui::Button("Place dummy enemy")) {
			placingDummy = true;
		}
		if (placingDummy) {
			Window::Draw(Assets::spritesheetTexture, Mouse::GetPositionInWorld())
				.withRectWithOriginCentered(AnimLib::GOOMBA[0].rect);
			if (Mouse::IsJustPressed()) {
				new Goomba(Mouse::GetPositionInWorld(), Goomba::Type::DUMMY);
				if (!Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
					placingDummy = false;
				}
			}
			if (Mouse::IsJustPressed(Mouse::Button::Right)) {
				placingDummy = false;
			}
		}
		if (ImGui::Button("Load")) {
			LoadGame();
			SceneManager::RestartScene();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save")) {
			SaveGame();
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear save")) {
			SaveState::Open(kSaveStateGameName, saveSlot)
				.Clear()
				.Save();
		}
		ImGui::End();
	}
#endif

	//for (int i = 0; i < Parallax::GetAll().size(); i++) {
	//	Parallax::GetAll()[i]->DrawImGUI(("parallax_" + std::to_string(i)).c_str());
	//}
	//Parallax::GetAll().back()->DrawImGUI("parallax3");
	//Parallax::GetAll()[Parallax::GetAll().size() - 2]->DrawImGUI("parallax2");
	//Parallax::GetAll()[Parallax::GetAll().size() - 3]->DrawImGUI("parallax1");

#ifdef _DEBUG
	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEditDraw();
	}
#endif

	Fx::FullscreenShader::Deactivate(); // Does nothing if no shader was active

	Camera::InScreenCoords::Begin();
	player.DrawGUI();
	int numBars = 0;
	for (const Bipedal* g : Bipedal::GetAll()) {
		numBars += g->DrawHealth(numBars);
	}
	for (const Minotaur* g : Minotaur::GetAll()) {
		numBars += g->DrawHealth(numBars);
	}
	for (const Ooy* g : Ooy::GetAll()) {
		numBars += g->DrawHealth(numBars);
	}
	for (const Mantis* g : Mantis::GetAll()) {
		numBars += g->DrawHealth(numBars);
	}
	player.DrawGUI();
	dialogDriver.Draw();

	if (playerPaused) {
		Window::Draw(Assets::spritesheetTexture, Window::GAME_WIDTH - 25, 25)
			.withScale(2.5f)
			.withRect(456, 308, 14, 19)
			.withOrigin(14, 0);
	}

	Camera::InScreenCoords::End();

	Fx::AfterDraw();

	//player.dust.DrawImGUI("Dust");
}
