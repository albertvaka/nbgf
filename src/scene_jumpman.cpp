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
#include "debug.h"
#include "collide.h"
#include "rototext.h"
#include "goomba.h"
#include "flyingalien.h"
#include "drawall.h"

extern float mainClock;

const float camSpeed = 2000;
const float introDuration = 1.f;

JumpScene::JumpScene()
	: map(TiledMap::map_size.x, TiledMap::map_size.y, Assets::marioTexture)
	, fogPartSys(Assets::fogTexture)
{
	Bullet::InitParticles();
	Missile::InitParticles();

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

	for (const Bounds& b : TiledAreas::parallax_forest) {
		new Parallax(b, Assets::forestParallaxTextures, 0.25f, 1.f, 172.f);
	}

	for (const Bounds& b : TiledAreas::parallax_island) {
		new Parallax(b, Assets::islandParallaxTextures, 0.f, 0.3f, 122.f);
	}

	for (const Bounds& b : TiledAreas::parallax_cave) {
		new Parallax(b, Assets::caveParallaxTextures, 0.4f, 0.65f, -165.f);
	}

	for (const auto& screen : TiledMap::screens) {
		screenManager.AddScreen(screen);
	}
}

JumpScene::~JumpScene() {
	Parallax::DeleteAll();
}

void JumpScene::EnterScene()
{
	player.Reset(TiledEntities::spawn);

	map.LoadFromTiled();

	for (auto const& [id, pos] : TiledEntities::bat) {
		new Bat(pos, false, false);
	}

	new Bipedal(TiledEntities::boss_bipedal);

	for (auto const& [id, pos] : TiledEntities::angrybat) {
		new Bat(pos, true, false);
	}

	for (auto const& [id, pos] : TiledEntities::batawake) {
		new Bat(pos, false, true);
	}

	for (auto const& [id, pos] : TiledEntities::fireslime) {
		new FireSlime(pos);
	}

	for (auto const& [id, pos] : TiledEntities::goomba) {
		new Goomba(pos, false);
	}

	for (auto const& [id, pos] : TiledEntities::goombacharger) {
		new Goomba(pos,true);
	}

	for (auto const& [id, pos] : TiledEntities::mantis) {
		new Mantis(pos);
	}

	for (auto const& [id, pos] : TiledEntities::flyingalien) {
		new FlyingAlien(pos);
	}

	//for (vec v : TiledEntities::save) {
	//	new SaveStation(v);
	//}


	new BigItem(TiledEntities::skill_walljump, Skill::WALLJUMP);
	new BigItem(TiledEntities::skill_gun, Skill::GUN);
	BigItem* break_skill = new BigItem(TiledEntities::skill_breakblocks, Skill::BREAK);

	int screen_break_skill = screenManager.FindScreenContaining(break_skill->pos);

	for (auto const& [id, pos] : TiledEntities::healthup) {
		new HealthUp(pos);
	}

	for (auto const& [id, pos] : TiledEntities::enemy_door) {
		EnemyDoor* d = new EnemyDoor(pos);
		int door_screen = screenManager.FindScreenContaining(d->pos);
		if (door_screen < 0) {
			Debug::out << "Warning: Enemy door outside a screen";
		}
		for (const Bat* b : Bat::GetAll()) {
			if (b->screen == door_screen) {
				d->AddEnemy(b);
			}
		}
		if (door_screen == screen_break_skill) {
			door_to_close_when_break_skill = d;
		}
	}

	for (const Bounds& a : TiledAreas::lava) {
		Lava* lava = new Lava(a);
		if (a.Contains(TiledEntities::lava_initial_height)) {
			raising_lava = lava;
			raising_lava_target_height = lava->CurrentLevel();
			lava->SetLevel(TiledEntities::lava_initial_height.y, true);
		}
	}

	screenManager.UpdateCurrentScreen(player.pos);
	Camera::SetCenter(GetCameraTargetPos());

	FxManager::StartIntroTransition(introDuration);
}

vec JumpScene::GetCameraTargetPos() {
	// TODO: keep the camera so you see a bit more in the direction you are going (like in https://youtu.be/AqturoCh5lM?t=3801)
	vec camPos = (player.pos * 17 + Mouse::GetPositionInWorld() * 2) / 19.f;
	screenManager.ClampCameraToScreen(camPos);
	return camPos;
}

void JumpScene::UpdateCamera(float dt) {
	vec camPos = GetCameraTargetPos();
	vec oldPos = Camera::GetCenter();
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
}
void JumpScene::Update(float dt)
{
	FxManager::Update(dt);
	if (FxManager::IsTheWorldStopped()) {
		return;
	}
	if (FxManager::IsIntroTransitionActive() || FxManager::IsOuttroTransitionActive()) {
		return;
	} else if (FxManager::IsOuttroTransitionDone()) {
		FxManager::ResetOuttroTransitionDone();
		// Restart scene
		ExitScene();
		EnterScene();
		return;
	}

	for (OneShotAnim* e : OneShotAnim::GetAll()) { // Update this first so one-frame anims aren't deleted before they are drawn once
		e->Update(dt);
	}
	OneShotAnim::DeleteNotAlive();

	skillTree.Update(dt);
	if (skillTree.open) return;

	player.Update(dt);
	if (!player.alive) {
		FxManager::StartOuttroTransition(introDuration);
	}

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
	if (Keyboard::IsKeyJustPressed(restart)) {
		FxManager::StartOuttroTransition(introDuration);
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
				e->die();
			}
		}
	}

	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		map.DebugEdit();
	}
#endif

	for (EnemyDoor* ed : EnemyDoor::GetAll()) {
		ed->Update(dt); // Checks for enemies with alive = false, crashes if they have been deleted already
	}

	Bat::DeleteNotAlive(); //Must happen after enemydoor update
	Goomba::DeleteNotAlive();
	FlyingAlien::DeleteNotAlive();
	Mantis::DeleteNotAlive();
	FireSlime::DeleteNotAlive();

	for (HealthUp* g : HealthUp::GetAll()) {
		if (Collide(g->bounds(), player.bounds())) {

			//TODO: PICK UP ANIMATION

			//TODO: Raise health

			g->alive = false;
		}
	}

	HealthUp::DeleteNotAlive();

	for (BigItem* g : BigItem::GetAll()) {
		if (Collide(g->bounds(), player.bounds())) {

			//TODO: PICK UP ANIMATION

			skillTree.Enable(g->skill);
			g->alive = false;

			switch(g->skill) {
			case Skill::WALLJUMP:
			{
				raising_lava->SetLevel(raising_lava_target_height);
				rotoText.ShowMessage("WallJump");
			}
				break;
			case Skill::GUN:
			{
				rotoText.ShowMessage("Big F. Gun");
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
				rotoText.ShowMessage("Shots that\nbreak stuff");
				door_to_close_when_break_skill->Lock();
			}
				break;
			default:
				break;
			}
		}
	}

	BigItem::DeleteNotAlive();

	Bullet::particles.UpdateParticles(dt);
	Missile::particles.UpdateParticles(dt);

	destroyedTiles.Update(dt);

	if (raising_lava->CurrentLevel() <= raising_lava_target_height+1.f) {
		raising_lava->SetLevel(TiledEntities::lava_initial_height.y);
	}
	for (Lava* l : Lava::GetAll()) {
		l->Update(dt);
	}
	if (player.frozen) {
		// stop lava to prevent it lowering and suddently us not being inside
		raising_lava->SetLevel(raising_lava->CurrentLevel());
	}

	contextActionButton = GameKeys::NONE;
	for (SaveStation* ss : SaveStation::GetAll()) {
		ss->Update(dt);
		if (ss->enabled && Collide(ss->bounds(), player.bounds())) {
			contextActionButton = GameKeys::ACTION;
			if (Input::IsJustPressed(0,GameKeys::ACTION)) {
				ss->Activate();
			}
		}
	}

	for (const Bounds& a : TiledAreas::fog) {
		if (!Collide(Camera::GetBounds(),(Bounds(a)*2.f))) {
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
		Bullet::GetAll(),
		FireShot::GetAll(),
		Missile::GetAll(),
		HealthUp::GetAll(),
		BigItem::GetAll(),
		&player,
		Lava::GetAll()
	);

	if (contextActionButton != GameKeys::NONE) {
		AnimationType anim = BUTTON_A_PRESS; // TODO: switch depending on the key
		Window::Draw(Assets::hospitalTexture, player.bounds().TopRight() + vec(2, -6))
			.withRect(Animation::AnimFrame(anim, mainClock * 1000));
	}

	rotoText.Draw(vec(0,-30));

#ifdef _IMGUI
	{
		ImGui::Begin("jumpman scene");
		//ImGui::SliderFloat("y", &player.pos.y, 0.f, 25 * 16.f);
		vec m = Mouse::GetPositionInWorld();
		veci t = map.toTiles(m);
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		if (ImGui::Button("Add point")) {
			skillTree.gunpoints += 1;
		};
		ImGui::Text("Mouse on tile: %d,%d", t.x, t.y);
		ImGui::SliderFloat("lava", &(Lava::GetAll()[0]->targetY), (TiledMap::map_size.y - 1) * 16, (TiledMap::map_size.y - 1) * 16 - 1000);
		ImGui::End();

		//FxManager::DrawImgui();
	}
#endif

	//Bullet::particles.DrawImGUI("BulletTrail");
	//Missile::particles.DrawImGUI("MissileSmoke");

	//for (const Bounds& a : TiledAreas::parallax_forest) {
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

	skillTree.DrawOverlay();

	//player.polvito.DrawImGUI("Polvito");
}
