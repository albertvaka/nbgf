#include <sstream>

#include "scene_jumpman.h"
#include "input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "bullet.h"
#include "enemy_door.h"
#include "assets.h"
#include "parallax.h"
#include "bat.h"
#include "missile.h"
#include "mantis.h"
#include "bipedal.h"
#include "fxmanager.h"
#include "fireslime.h"
#include "fireshot.h"
#include "lava.h"
#include "savestation.h"
#include "common_enemy.h"
#include "debug.h"
#include "collide.h"
#include "rototext.h"
#include "goomba.h"
#include "flyingalien.h"
#include "drawall.h"
#include "savestate.h"
#include "bigitem.h"
#include "health.h"
#include "healthup.h"

extern float mainClock;

const float camSpeed = 2000;

const char* kSaveStateGameName = "gaem2020";

JumpScene::JumpScene(int saveSlot)
	: map(TiledMap::map_size.x, TiledMap::map_size.y, Assets::marioTexture)
	, rotoText(Assets::font_30, Assets::font_30_outline)
	, fogPartSys(Assets::fogTexture)
	, saveSlot(saveSlot)
{
	Bullet::InitParticles();
	Missile::InitParticles();
	Health::InitParticles();

	fogPartSys.AddSprite({ 0, 0, 140, 61 });
	fogPartSys.AddSprite({140, 0, 140, 61 });
	fogPartSys.AddSprite({140*2, 0, 140, 61 });
	fogPartSys.AddSprite({140 * 3, 0, 140, 61 });
	fogPartSys.alpha = 0.f;
	fogPartSys.alpha_vel = 0.1f;
	fogPartSys.bounce_alpha = 0.5f;
	fogPartSys.min_scale = 2.f;
	fogPartSys.max_scale = 3.f;
	fogPartSys.min_vel.x = 17.f;
	fogPartSys.max_vel.x = 20.f;
	fogPartSys.min_ttl = fogPartSys.max_ttl = 10.f;
	fogPartSys.min_interval = 4.5f;
	fogPartSys.max_interval = 6.f;

	for (const BoxBounds& b : TiledAreas::parallax_forest) {
		new Parallax(b, Assets::forestParallaxTextures, 0.25f, 1.f, 172.f);
	}

	for (const BoxBounds& b : TiledAreas::parallax_island) {
		new Parallax(b, Assets::islandParallaxTextures, 0.f, 0.3f, 122.f);
	}

	for (const BoxBounds& b : TiledAreas::parallax_cave) {
		new Parallax(b, Assets::caveParallaxTextures, 0.4f, 0.65f, -165.f);
	}

	for (const auto& screen : TiledMap::screens) {
		screenManager.AddScreen(screen);
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

	for (EnemyDoor* g : EnemyDoor::GetAll()) {
		g->SaveGame(saveState);
	}

	skillTree.SaveGame(saveState);

	saveState.StreamPut("bossdead_bipedal") << (boss_bipedal == nullptr);

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

	for (EnemyDoor* g : EnemyDoor::GetAll()) {
		g->LoadGame(saveState);
	}

	skillTree.LoadGame(saveState);
	for (BigItem* g : BigItem::GetAll()) {
		if (skillTree.IsEnabled(g->skill)) {
			TriggerPickupItem(g, true);
			g->alive = false;
		}
	}

	bool bossdead_bipedal = false;
	saveState.StreamGet("bossdead_bipedal") >> bossdead_bipedal;
	if (bossdead_bipedal) {
		boss_bipedal->alive = false;
	}

	Update(0); //Hackish way to make sure the entities with alive=false trigger things on other components before being deleted

	player.LoadGame(saveState);
}

void JumpScene::TriggerPickupItem(BigItem* g, [[maybe_unused]] bool fromSave) {

	switch (g->skill) {
	case Skill::WALLJUMP:
	{
		raising_lava->SetLevel(raising_lava_target_height);
	}
	break;
	case Skill::GUN:
	{
		int screen_gun = screenManager.FindScreenContaining(g->pos);
		for (Bat* e : Bat::GetAll()) {
			if (e->screen == screen_gun) {
				e->awakened = true;
			}
		}
		for (auto const& [id, pos] : TiledEntities::initial_batawake) {
			Bat* b = new Bat(pos, false, true);
			door_to_close_when_break_skill->AddEnemy(b);
		}
	}
	break;
	case Skill::BREAK:
	{
		door_to_close_when_break_skill->Lock();
	}
	break;
	default:
		break;
	}


}
void JumpScene::EnterScene()
{
	player.Reset(TiledEntities::spawn);

	map.LoadFromTiled();

	new BigItem(TiledEntities::skill_walljump, Skill::WALLJUMP);
	new BigItem(TiledEntities::skill_gun, Skill::GUN);
	BigItem* break_skill = new BigItem(TiledEntities::skill_breakblocks, Skill::BREAK);

	int screen_break_skill = screenManager.FindScreenContaining(break_skill->pos);

	for (auto const& [id, pos] : TiledEntities::save) {
		new SaveStation(id, pos);
	}

	for (auto const& [id, pos] : TiledEntities::enemy_door) {
		EnemyDoor* d = new EnemyDoor(id, pos);
		int door_screen = screenManager.FindScreenContaining(d->pos);
		if (door_screen == screen_break_skill) {
			door_to_close_when_break_skill = d;
		}
	}

	for (auto const& [id, pos] : TiledEntities::bat) {
		Bat* b = new Bat(pos, false, false);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : TiledEntities::angrybat) {
		Bat* b = new Bat(pos, true, false);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : TiledEntities::batawake) {
		Bat* b =new Bat(pos, false, true);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : TiledEntities::fireslime) {
		auto b = new FireSlime(pos);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : TiledEntities::goomba) {
		auto b = new Goomba(pos, false);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : TiledEntities::goombacharger) {
		auto b = new Goomba(pos,true);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : TiledEntities::mantis) {
		auto b = new Mantis(pos);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	for (auto const& [id, pos] : TiledEntities::flyingalien) {
		auto b = new FlyingAlien(pos);
		for (EnemyDoor* s : EnemyDoor::ByScreen[b->screen]) {
			s->AddEnemy(b);
		}
	}

	boss_bipedal = new Bipedal(TiledEntities::boss_bipedal);

	for (SaveStation* s : SaveStation::ByScreen[screenManager.FindScreenContaining(boss_bipedal->pos)]) {
		s->AddHiddenBy(boss_bipedal);
	}

	for (auto const& [id, pos] : TiledEntities::healthup) {
		new HealthUp(id, pos);
	}

	for (const BoxBounds& a : TiledAreas::lava) {
		Lava* lava = new Lava(a);
		if (a.Contains(TiledEntities::lava_initial_height)) {
			raising_lava = lava;
			raising_lava_target_height = lava->CurrentLevel();
			lava->SetLevel(TiledEntities::lava_initial_height.y, true);
		}
	}

	LoadGame();

	Camera::SetCenter(GetCameraTargetPos());

	FxManager::StartTransition(Assets::fadeInDiamondsShader);
}

vec JumpScene::GetCameraTargetPos() {
	// TODO: keep the camera so you see a bit more in the direction you are going (like in https://youtu.be/AqturoCh5lM?t=3801)
	vec camPos = (player.pos * 17 + Mouse::GetPositionInWorld() * 2) / 19.f;
	screenManager.ClampCameraToScreen(camPos);
	return camPos;
}

void JumpScene::UpdateCamera(float dt) {
	vec camPos = GetCameraTargetPos();
	vec oldPos = Camera::Center();
	vec displacement = camPos - oldPos;
	displacement.Truncate(camSpeed * dt);
	Camera::SetCenter(oldPos + displacement + FxManager::GetScreenshake());
}

void JumpScene::ExitScene()
{
	Bullet::DeleteAll();
	Bullet::particles.Clear();
	Missile::DeleteAll();
	Missile::particles.Clear();
	FireShot::DeleteAll();
	Bat::DeleteAll();
	Goomba::DeleteAll();
	Mantis::DeleteAll();
	FlyingAlien::DeleteAll();
	FireSlime::DeleteAll();
	OneShotAnim::DeleteAll();
	Bipedal::DeleteAll();
	Lava::DeleteAll();
	destroyedTiles.Clear();
	EnemyDoor::DeleteAll();
	BigItem::DeleteAll();
	HealthUp::DeleteAll();
	Health::DeleteAll();
	Health::particles.Clear();
	SaveStation::DeleteAll();
}

void JumpScene::Update(float dt)
{
	FxManager::Update(dt);
	if (FxManager::IsTheWorldStopped()) {
		return;
	}
	if (FxManager::IsTransitionActive()) {
		return;
	} else if (FxManager::IsTransitionDone()) {
		FxManager::ResetTransitionDone();
		if (FxManager::GetCurrentTransition() != &Assets::fadeInDiamondsShader) {
			// This was a death or outro transition: restart scene
			ExitScene();
			EnterScene();
		}
		return;
	}

	if (player.health <= 0) {
		//Assets::soundDeath.Play();
		vec normalizedPlayerPos = Camera::WorldToScreen(player.Bounds().Center()) / Camera::InScreenCoords::Size();
		Assets::fadeOutCircleShader.Activate(); // Must be active to set uniforms
		Assets::fadeOutCircleShader.SetUniform("normalizedTarget", normalizedPlayerPos);
		Shader::Deactivate();
		FxManager::StartTransition(Assets::fadeOutCircleShader);
		return;
	}

	skillTree.Update(dt);
	if (skillTree.open) return; // Pause menu

	contextActionButton = false;

	for (OneShotAnim* e : OneShotAnim::GetAll()) { // Update this first so one-frame anims aren't deleted before they are drawn once
		e->Update(dt);
	}
	OneShotAnim::DeleteNotAlive();

	player.Update(dt);

	screenManager.UpdateCurrentScreen(player.pos);

	UpdateCamera(dt);

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
	
	for (Missile* e  : Missile::GetAll()) {
		e->Update(dt);
	}
	Missile::DeleteNotAlive();

	for (Bat* e : Bat::GetAll()) {
		e->Update(dt);
	}

	for (Goomba* e : Goomba::GetAll()) {
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
	const SDL_Scancode shift = SDL_SCANCODE_LSHIFT;
	if (Keyboard::IsKeyJustPressed(restart)) {
		if (Keyboard::IsKeyPressed(shift)) {
			SaveState saveState = SaveState::Open(kSaveStateGameName, saveSlot);
			saveState.Clear();
			saveState.Save();
		}
		FxManager::StartTransition(Assets::fadeOutDiamondsShader);
		return;
	}
	if (Keyboard::IsKeyJustPressed(teleport)) {
		player.pos = TiledEntities::debug_teleport;
		screenManager.UpdateCurrentScreen(player.pos);
		Camera::SetCenter(GetCameraTargetPos());
	}
	if (Keyboard::IsKeyJustPressed(unlockbasics)) {
		skillTree.Enable(Skill::GUN);
		skillTree.Enable(Skill::WALLJUMP);
		skillTree.Enable(Skill::BREAK);
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
				FxManager::StartTransition(Assets::fadeOutDiamondsShader);
			}
		}
	}

	if (boss_bipedal && !boss_bipedal->alive) {
		boss_bipedal = nullptr;
	}

	// Delete enemies after updating doors and savestations that might check for them
	Bat::DeleteNotAlive();
	Goomba::DeleteNotAlive();
	FlyingAlien::DeleteNotAlive();
	Mantis::DeleteNotAlive();
	FireSlime::DeleteNotAlive();
	Bipedal::DeleteNotAlive();

	for (HealthUp* g : HealthUp::GetAll()) {
		g->Update(dt);
	}

	for (Health* g : Health::GetAll()) {
		g->Update(dt);
	}
	Health::DeleteNotAlive();

	for (BigItem* g : BigItem::GetAll()) {
		if (Collide(g->Bounds(), player.Bounds())) {
			skillTree.Enable(g->skill);
			
			//TODO: Pickup animation or popup or something
			switch(g->skill) {
				case Skill::WALLJUMP:
					rotoText.ShowMessage("WallJump");
					break;
				case Skill::GUN:
					rotoText.ShowMessage("Big F. Gun");
					break;
				case Skill::BREAK:
					rotoText.ShowMessage("Shots that\nbreak stuff");
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

	Bullet::particles.UpdateParticles(dt);
	Missile::particles.UpdateParticles(dt);
	Health::particles.UpdateParticles(dt);

	destroyedTiles.Update(dt);

	if (raising_lava->CurrentLevel() <= raising_lava_target_height+1.f) {
		raising_lava->SetLevel(TiledEntities::lava_initial_height.y);
	}
	for (Lava* l : Lava::GetAll()) {
		l->Update(dt);
	}

	for (const BoxBounds& a : TiledAreas::fog) {
		if (!Collide(Camera::Bounds(),(a*2.f))) {
			continue;
		}
		for (int x = 50; x < a.width - 180; x += 50) {
			for (int y = 0; y < a.height; y += 50) {
				fogPartSys.pos = vec(a.left + x, a.top + y);
				fogPartSys.Spawn(dt);
			}
		}
		fogPartSys.UpdateParticles(dt);
	}

	rotoText.Update(dt);

}

void JumpScene::Draw()
{
	if (skillTree.open) {
		skillTree.DrawMenu();
		return;
	}

	FxManager::BeginDraw();

	Window::Clear(31, 36, 50);

	DrawAllInOrder(
		Parallax::GetAll(),
		&map,
		&destroyedTiles,
		SaveStation::GetAll(),
		EnemyDoor::GetAll(),
		OneShotAnim::GetAll(),
		Goomba::GetAll(),
		Mantis::GetAll(),
		FlyingAlien::GetAll(),
		FireSlime::GetAll(),
		Bat::GetAll(),
		Bipedal::GetAll(),
		&Bullet::particles,
		&Missile::particles,
		Health::GetAll(),
		&Health::particles,
		Bullet::GetAll(),
		FireShot::GetAll(),
		Missile::GetAll(),
		HealthUp::GetAll(),
		BigItem::GetAll(),
		&player,
		Lava::GetAll()
	);

	if (contextActionButton) {
		Window::Draw(Assets::hospitalTexture, player.Bounds().TopRight() + vec(2, -6))
			.withRect(Animation::GetRectAtTime(AnimLib::BUTTON_A_PRESS, mainClock));
	}

	rotoText.Draw(vec(0,-30));

#ifdef _IMGUI
	{
		ImGui::Begin("jumpman scene");
		ImGui::SliderFloat2("player", (float*)&player.pos, 16.f, 4500.f);
		vec m = Mouse::GetPositionInWorld();
		veci t = map.ToTiles(m);
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		ImGui::Text("Mouse tile: %d,%d", t.x, t.y);
		ImGui::SliderFloat("lava", &(Lava::GetAll()[0]->targetY), (TiledMap::map_size.y - 1) * 16, (TiledMap::map_size.y - 1) * 16 - 1000);


		if (ImGui::Button("Start waves")) {
			FxManager::ActivateShaderFullscreen([]() {
				Assets::waveShader.Activate();
				Assets::waveShader.SetUniform("time", mainClock);
			});
		}
		if (ImGui::Button("Stop waves")) {
			FxManager::DeactivateShaderFullscreen();
		}


		if (ImGui::Button("Save")) {
			SaveGame();
		}
		if (ImGui::Button("Load in same scene")) {
			LoadGame();
		}
		if (ImGui::Button("Load in new scene")) {
			LoadGame();
			ExitScene();
			EnterScene();
		}
		if (ImGui::Button("Clear save")) {
			SaveState::Open(kSaveStateGameName, saveSlot)
				.Clear()
				.Save();
		}
		ImGui::End();
	}
	//FxManager::DrawImgui();
#endif

	//Health::particles.DrawImGUI("health");
	//Bullet::particles.DrawImGUI("BulletTrail");
	//Missile::particles.DrawImGUI("MissileSmoke");

	//for (const BoxBounds& a : TiledAreas::parallax_forest) {
		//Assets::fogShader.Activate();
		//Assets::fogShader.SetUniform("offset", vec(mainClock*0.2f, 0.f));
		//Assets::fogShader.SetUniform("time", mainClock);
		// The texture is not used by the shader at all
		//Window::Draw(Assets::hospitalTexture, a);
		//Shader::Deactivate();
	//}

	//fogPartSys.Draw();
	//fogPartSys.DrawImGUI();

	//for (int i = 0; i < Parallax::GetAll().size(); i++) {
	//	Parallax::GetAll()[i]->DrawImGUI(("parallax_" + std::to_string(i)).c_str());
	//}

#ifdef _DEBUG
	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEditDraw();
	}
#endif

	FxManager::EndDraw();

	Camera::InScreenCoords::Begin();
	player.DrawGUI();
	Camera::InScreenCoords::End();

	//player.polvito.DrawImGUI("Polvito");
}
