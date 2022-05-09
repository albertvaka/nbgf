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
#include "enemies_by_screen.h"

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
	, text_holi(Assets::font_30, Assets::font_30_outline)
	, text_holi_2(Assets::font_30, Assets::font_30_outline)
	, text_frame_by_frame(Assets::font_30, Assets::font_30_outline)
	, text_debug_draw(Assets::font_30, Assets::font_30_outline)
	, text_move_cam(Assets::font_30, Assets::font_30_outline)
	, text_imgui(Assets::font_30, Assets::font_30_outline)
	, saveSlot(saveSlot)
{
	Particles::Init();

	for (const BoxBounds& b : Tiled::Areas::parallax_forest) {
		new Parallax(b, Assets::forestParallaxTextures, 0.25f, 1.f, 172.f);
	}

	for (const auto& screen : Tiled::Screens::screen) {
		int id = ScreenManager::AddScreen(screen);
	}
	text_holi.SetString("NBGF: Debug features");
	text_holi_2.SetString("Albert Vaca Cintora <albertvaka@gmail.com>");
	text_frame_by_frame.SetString("Frame-by-frame mode");
	text_debug_draw.SetString("Debug view");
	text_move_cam.SetString("Free camera\nmode");
	text_move_cam.SetMultilineAlignment(Text::MultilineAlignment::CENTER);
	text_imgui.SetString("Dear ImGUI!");
}

JumpScene::~JumpScene() {
	Parallax::DeleteAll();
	ScreenManager::DeleteAllScreens();
}


void JumpScene::TriggerPickupItem(BigItem* g, [[maybe_unused]] bool fromSave) {

}

void JumpScene::EnterScene()
{
	Fx::BeforeEnterScene();

	player.Reset(Tiled::Entities::single_spawn, kPlayerInitialHealth);
	skillTree.Reset();
	skillTree.Enable(Skill::ATTACK);
	skillTree.Enable(Skill::DIVE);
	skillTree.Enable(Skill::DASH);
	skillTree.Enable(Skill::WALLJUMP);
	skillTree.Enable(Skill::BREAK);


	ScreenManager::UpdateCurrentScreen(player.pos);

	map.LoadFromTiled<Tiled::TileMap>();

	// FIXME: NPC
	//(new ForegroundOneShotAnim(Assets::warriorTexture, Tiled::Entities::single_npc, AnimLib::NPC_IDLE, 1.2f))->anim.loopable = true;

	for (auto const& [id, pos] : Tiled::Entities::bat) {
		new Bat(pos, false, false);
	}

	for (auto const& [id, pos] : Tiled::Entities::fireslime) {
		new FireSlime(pos);
	}

	for (auto const& [id, pos] : Tiled::Entities::rocket_launcher) {
		new RocketLauncher(pos);
	}

	for (auto const& [id, pos] : Tiled::Entities::miniooy) {
		new MiniOoy(pos);
	}


	for (auto const& [id, pos] : Tiled::Entities::save) {
		new SaveStation(id, pos);
	}

	for (const BoxBounds& a : Tiled::Areas::bg_bird_spawner) {
		new BackgroundBirdSpawner(a);
	}

	for (auto const& [id, pos] : Tiled::Entities::enemy_door) {
		new EnemyDoor(id, pos);
	}

	// Enemies <-> EnemyDoor
	for (auto const& [screen, doors] : EnemyDoor::ByScreen) {
		for (EnemyDoor* door : doors) {
			for (Entity* enemy : EnemiesByScreen::Get(screen)) {
				door->AddEnemy(enemy);
			}
		}
	}

	// Enemies <-> SaveStation
	for (auto const& [screen, stations] : SaveStation::ByScreen) {
		for (SaveStation* station : stations) {
			for (Entity* enemy : EnemiesByScreen::Get(screen)) {
				vec dist = enemy->pos.ManhattanDistance(station->pos);
				if (dist.x < Window::GAME_WIDTH / 2 && dist.y < Window::GAME_HEIGHT / 2) {
					station->AddEnemy(enemy);
				}
			}
		}
	}

	for (const BoxBounds& a : Tiled::Areas::lava) {
		Lava* lava = new Lava(a);
		raising_lava = lava;
		raising_lava->SetRaiseSpeed(60.f);
	}

	Camera::SetCenter(player.GetCameraTargetPos());

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
	EnemiesByScreen::AssertEmpty();
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

	ScreenManager::UpdateCurrentScreen(player.pos);

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
			if (e->screen == ScreenManager::CurrentScreen()) {
				DieWithSmallExplosion(e);
			}
		}
		for (FireSlime* e : FireSlime::GetAll()) {
			if (e->screen == ScreenManager::CurrentScreen()) {
				DieWithSmallExplosion(e);
			}
		}
	}

	// Sound tests
	//if (Mouse::IsJustPressed()) {
	//	Debug::out << Assets::soundMegazero.Play(Mouse::GetPositionInWorld(), player.pos);
	//}
	//if (Keyboard::IsKeyJustPressed(SDL_SCANCODE_M)) {
	//	Debug::out << Assets::soundMegazero.Play();
	//}

	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEdit();
	}
#endif

	for (EnemyDoor* ed : EnemyDoor::GetAll()) {
		// If clossed, it checks for enemies with alive == false to open
		ed->Update(dt);
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
		if (screen == ScreenManager::CurrentScreen()) {
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

	Window::Draw(text_holi, Tiled::Entities::single_holi).withScale(0.5f);
	Window::Draw(text_holi_2, Tiled::Entities::single_holi_2).withScale(0.25f);
	Window::Draw(text_frame_by_frame, Tiled::Entities::single_frame_by_frame).withScale(0.4f);
	Window::Draw(text_debug_draw, Tiled::Entities::single_debug_draw).withScale(0.4f);
	Window::Draw(text_move_cam, Tiled::Entities::single_move_cam).withScale(0.4f);
	Window::Draw(text_imgui, Tiled::Entities::single_imgui).withScale(0.4f);
	
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
		ImGui::Text("Mouse: %f,%f Tile: %d,%d", m.x, m.y, t.x, t.y);
		if (ImGui::Button("Start NPC dialog")) {
			dialogDriver.StartDialog(dialogWithRandomNpc);
		}

		static bool invincible = false;
		ImGui::Checkbox("invincible", &invincible);
		if (invincible) {
			*(const_cast<int*>(&player.health)) = std::max(player.health, player.maxHealth);
		}
		ImGui::SliderInt("health", const_cast<int*>(&player.health), 0, 7);
		ImGui::SliderInt("max_health", const_cast<int*>(&player.maxHealth), 0, 7);
		static float intial_lava_height = raising_lava->CurrentLevel();
		static float current_lava_height = raising_lava->CurrentLevel();
			if (ImGui::SliderFloat("lava_height", &current_lava_height, 1736, intial_lava_height)) {
				raising_lava->SetLevel(current_lava_height);
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
